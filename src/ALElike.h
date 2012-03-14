/*
 * Copyright (C) 2010,2011,2012 Scott Clark. All rights reserved.
 *
 * Developed by:
 * Scott Clark
 * Cornell University Center for Applied Mathematics
 * http://cam.cornell.edu
 * AND
 * Rob Egan
 * Department of Energy Joint Genome Institute
 * http://jgi.doe.gov
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal with the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 *   1. Redistributions of source code must retain the above copyright notice, 
 *      this list of conditions and the following disclaimers.
 *   2. Redistributions in binary form must reproduce the above copyright 
 *      notice, this list of conditions and the following disclaimers in the 
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the names of Cornell University, The Joint Genome Institute, 
 *      nor the names of its contributors may be used to endorse or promote 
 *      products derived from this Software without specific prior written 
 *      permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS WITH THE SOFTWARE.
 */

// For more information on the licence please see 
// The University of Illinois/NCSA Open Source License
// http://www.opensource.org/licenses/UoI-NCSA.php

// ALElike.h

#ifndef _ALE_LIKE_H_
#define _ALE_LIKE_H_

#include "ALEhelpers.h"
#include <math.h>

// casts a single numeric char to its int
int hackedIntCast(char c);

//uses Stirlings approximation to high precision
double lnfact2(double input);

// finds the insert probability (assuming normal distribution) P(point | N(0,sigma))
double GetInsertProbNormal(double point, const double sigma);

// finds or sets the MD field in a bam record
char *getMD(bam1_t *bam, char *ref);

double getInsertLikelihoodBAM(bam1_t *read1, double mu, double var);

// finds the likelihood of a string of misses in read from seqPos to matchLen
double loglikeMiss(char *readQual, int seqPos, int missLen, int qOff);

// finds the likelihood of a string of matches in read from seqPos to matchLen
double loglikeMatch(char *readQual, int seqPos, int matchLen, int qOff);

// finds the likelihood of an insertion (right now it is the same as a miss)
double loglikeInsertion(char *readQual, int seqPos, int insertionLength, int qOff);

// finds the likelihood of an deletion (right now it is the same as a miss)
double loglikeDeletion(char *readQual, int seqPos, int deletionLength, int qOff);

/*
// used to reduce likelihood in case of missmatches only
// (CIGAR already has accounted for matchlength, inserts, deletions)
//double getMDLogLikelihood(char *MD, char *readQual, int qOff);

//double getCIGARLogLikelihoodBAM(int numCigarOperations, uint32_t *cigar, char *readQual, int qOff, int *inserts, int *deletions, int *totalMatch);

*/

// prepares per reference base depth and placements using CIGAR and MD fields
void getContributionsForPositions(bam1_t *read, char *contigSeq, int qOff, int alignmentLength, float *depthPositions, double *loglikelihoodPositions);

// takes in a read and returns the match likelihood (due to matches, mismatches, indels)
//double getMatchLogLikelihoodBAM(bam1_t *read, int qOff, char *md);
double getMatchLogLikelihoodBAM(bam1_t *read, char *contigSeq, int qOff, int alignmentLength);

// returns the 2-bit hash representation of a nucl. given its place in the kmer
int kmerHash(char c1, int place);

// builds up a 2*kmerLen-bit hash (for the seq) given a seqenence and starting position
int getKmerHash(char *seq, int startPos, int kmerLen);

// calculates the kmer statistics
void computeKmerStats(assemblyT *theAssembly, int kmer);

// used in getPlacementWinner
unsigned int JSHash(char* str);

// finds the sum of the total likelihoods given the head of the list
double getTotalLikelihood(alignSet_t *head);

// find the placement winner
alignSet_t *getPlacementWinner(alignSet_t *head, double likeNormalizer, int *winner);

// apply statistics
int applyDepthAndMatchToContig(alignSet_t *alignment, assemblyT *theAssembly, double likeNormalizer, int qOff);

// this applies the placement(s) to the assembly part (SINGLE PART)
// I feel like this could be sped up with a hash table vs the current linked lists, but we will see...
int applyPlacement(alignSet_t *head, assemblyT *theAssembly, int qOff);

// compute the depth statistics
int computeDepthStats(assemblyT *theAssembly);

int guessQualityOffset(bam1_t *read);
        
static int mateTreeCount = 0;

int compare(int a, int b);
int mateTreeCmp(const void *pa, const void *pb);
int mateTreeCmpForceStore(const void *pa, const void *pb);

bam1_t *getOrStoreMate(void **mateTree1, void **mateTree2, bam1_t *thisRead);

void mateTreeApplyRemainderPlacement(const void *nodep, const VISIT which, const int depth);

void mateTreeFreeNode(void *nodep);

int isValidInsertSize(bam1_t *thisRead, libraryMateParametersT *mateParameters);

void validateAlignmentMates(alignSet_t *thisAlignment, bam1_t *thisRead, bam1_t *thisReadMate);

void _setAlignmentForMate(alignSet_t *thisAlignment, bam1_t *read1);

void _setAlignment(alignSet_t *thisAlignment, bam1_t *read1, bam1_t *read2);

enum MATE_ORIENTATION setAlignment(bam_header_t *header, assemblyT *theAssembly, alignSet_t *thisAlignment, void **mateTree1, void **mateTree2, libraryParametersT *libParams, enum MATE_ORIENTATION orientation, bam1_t *thisRead);

// divide by the expected likelihood of the read by the normalization factor Z (from Bayes rule)
// given only its length and the parameters of the distributions (See paper appendix)
double logzNormalizationReadQual(bam1_t *thisRead, int qOff);

void computeReadPlacements(samfile_t *ins, assemblyT *theAssembly, libraryParametersT *libParams, samfile_t *placementBam);

#ifndef _GNU_SRC
void tdestroy(void *root, void (*free_node)(void *nodep));
#endif

#endif
