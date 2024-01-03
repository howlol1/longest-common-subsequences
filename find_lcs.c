#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char** subsequences; // Array of strings
    int count; // Number of subsequences
} SubsequenceArray;



int** getDPMatrix(char* str1, char* str2) {
	// DP matrix will have extra 1 row & column
    int N = strlen(str1) + 1; 
    int M = strlen(str2) + 1;
    int i,j;
    int** matrix = malloc(N * sizeof(int*));
    // Set first row and first column values to 0
    for (i = 0; i < N; i++) {
        matrix[i] = malloc(M * sizeof(int));
        matrix[i][0] = 0;
    }
    for (j = 0; j < M; j++) {
        matrix[0][j] = 0; 
    }
    
    // Formula for calculating matrix elements:
    // M[i,j] = 0 if i==0 or j==0
    // M[i,j] = M[i-1,j-1] + 1 if str1[i-1] == str2[j-1]
    // M[i,j] = max(M[i-1,j], M[i,j-1] if str1[i-1] != str2[j-1]
    
    for (i = 1; i < N; i++) {
        for (j = 1; j < M; j++) {
            if (str1[i - 1] == str2[j - 1]) {
                matrix[i][j] = 1 + matrix[i - 1][j - 1];
            } else {
                matrix[i][j] = (matrix[i - 1][j] > matrix[i][j - 1]) ? matrix[i - 1][j] : matrix[i][j - 1];
            }
        }
    }
    return matrix;
}

// Helper function to print the constructed DP matrix
void printDPMatrix(int** matrix, char* str1, char* str2){
	int i,j;
	int N = strlen(str1) + 1;
	int M = strlen(str2) + 1;
	printf("    ");
	for(j=1; j<M; j++){
		printf("%c ", str2[j-1]);
	}
	for(i=0; i<N;i++){
		printf("\n");
		if(i!=0){
			printf("%c ",str1[i-1]);
		} else {
			printf("  ");
		}
		for(j=0;j<M;j++){
			printf("%d ", matrix[i][j]);
		}
	}
	printf("\n\n");
}

// Helper function to reverse a given string because our subsequences are formed in a reverse order.
char* reverseString(const char* str) {
	int i;
    int len = strlen(str);
    char* reversed = malloc(len + 1); // +1 for the null terminator

    if (reversed) {
        for (i = 0; i < len; ++i) {
            reversed[i] = str[len - 1 - i];
        }
        reversed[len] = '\0'; // Null-terminating the string
    }

    return reversed;
}


// findNextMatchUp finds the next matching letter indices given a specific row & col number.
// When we are on a cell that is not a matching letter and left & above cells are equal function goes to above cell (i-1).
void findNextMatchUp(int** DPMatrix, char* str1, char* str2, int i, int j, int nextIndices[2]) {
	while(i>0 && j>0){
		
		if(str1[i-1] == str2[j-1]){
			// Return the indices where str1[i-1] = str2[j-1] (matching letters) 
			nextIndices[0] = i;
			nextIndices[1] = j;
			return;
		} else{
			if(DPMatrix[i-1][j] > DPMatrix[i][j-1]){
				i--;
			} else if(DPMatrix[i][j-1] > DPMatrix[i-1][j]){
				j--;
			} else {
				i--; // Look for above cell.
			}
		}
		
	}
	// No match on this branch
    nextIndices[0] = -1;
    nextIndices[1] = -1;
}

// Similar to findNextMatchUp this function looks for the left cell when there is no matching letters and left & above cells are equal.
void findNextMatchLeft(int** DPMatrix, char* str1, char* str2, int i, int j, int nextIndices[2]) {
	while(i>0 && j>0){
		
		if(str1[i-1] == str2[j-1]){
			nextIndices[0] = i;
			nextIndices[1] = j;
			return;
		} else{
			if(DPMatrix[i-1][j] > DPMatrix[i][j-1]){
				i--;
			} else if(DPMatrix[i][j-1] > DPMatrix[i-1][j]){
				j--;
			} else {
				j--; // Look for left cell.
			}
		}
		
	}
	// No match on this branch
    nextIndices[0] = -1;
    nextIndices[1] = -1;
}


// subsequence variable is for holding the appended letters at each subsequence (branch).
// subseqCount variable holds the number of different branches containing unique subsequences.
// subsequenceArray is to hold each subsequence to be used later on.
// i,j is respectively represents rows & columns which we are on DP matrix.

// Function starts from the last element of matrix i = len(str1), j = len(str2) if a matching letter is found it is added to subsequence.
// If no matching letter is found we continue from the cell that has bigger value among [i-1,j] and [i,j-1]
// If those both cells are equal there is a possible branch here (unique subsequences)
// We utilize helper functions findNextMatchUp and findNextMatchLeft to identify if this branching would lead to different subsequences.
// When there is equity within a no matching-letter cell findNextMatchUp takes above cell and findNextMatchLeft takes left cell.
// findNextMatchup is being called for [i-1][j] branch findNextMatchLeft is being called for [i][j-1] branch.
// If the returned indices from findNextMatchup and findNextMatchLeft corresponds to same letter this means those branches are duplicates.
// This way we eliminate the possibility of identifying unique subsequences as converging branches.
// Branches only converge if there is no possible splits. Otherwise we make our split by adding a branch on [i][j-1].
// Function traverses DP Matrix recursively using this branching logic and returns unique subsequences.
void findSubsequences(int** DPMatrix, char* str1, char* str2, int i, int j, char* subsequence, int* subseqCount, SubsequenceArray* subsequenceArray){
	if(i==0 || j==0){
		// Reverse subsequence since we are traversing the DP Matrix and constructing subsequence in reverse order.
		// Add each returned subsequence from branches after reversing to SubsequenceArray.
		
		if(subsequence==NULL){
			return;
		}
		int k = 0;
		int uniqueSubsequence = 1;
		subsequence = reverseString(subsequence);
		
		// Check if this subsequence has been added before
		while(k<(*subseqCount) && uniqueSubsequence == 1){
			if(strcmp(subsequenceArray->subsequences[k], subsequence) == 0){
				printf("%s subsequence has been added before\n",subsequence, *subseqCount);
				uniqueSubsequence=0;
			}
			k++;
		}
		
		if(uniqueSubsequence == 1){
			subsequenceArray->subsequences = realloc(subsequenceArray->subsequences, (*subseqCount + 1) * sizeof(char*));
			subsequenceArray->subsequences[*subseqCount] = strdup(subsequence);
			printf("%s = Sequence %d \n",subsequence,*subseqCount+1);
			(*subseqCount)++;
			subsequenceArray->count = *subseqCount;
		}
		
		return;
	}
	// If str1[i-1] and str2[j-1] are matching add the letter to subsequence.
	if(str1[i-1] == str2[j-1]){
		// Matching letters add letter to subsequence and move to i-1, j-1
		printf("Appending %c to subsequence %d, Prefix: %s\n",str1[i-1],*subseqCount+1, subsequence);
		char new_subsequence[strlen(subsequence) + 2];
        strcpy(new_subsequence, subsequence);
        new_subsequence[strlen(subsequence)] = str1[i - 1];
        new_subsequence[strlen(subsequence) + 1] = '\0';
		findSubsequences(DPMatrix, str1, str2, i-1, j-1, new_subsequence, subseqCount, subsequenceArray);
	}
	else
		// No matching letters check if above cell is equal to left cell to see if it is a possible branching point.
		if(DPMatrix[i-1][j] == DPMatrix[i][j-1]){
			// Possible branch
			int nextIndices1[2], nextIndices2[2];
			// Branch 1 calls findNextMatchUp function and we return the indices of next matching letter on this branch to nextIndices1
			// Branch 2 calls findNextMatchLeft function and we return the indices of next matching letter on this branch to nextIndices2
       		findNextMatchUp(DPMatrix, str1, str2, i-1, j, nextIndices1);
        	findNextMatchLeft(DPMatrix, str1, str2, i, j-1, nextIndices2);
        	printf("Possible branch at %d, %d\n", i,j); // Debug statement
        	// If indices returned from both functions are equal that means Branch 1 and Branch 2 are duplicates otherwise they lead to unique subsequences.
        	if (nextIndices1[0] != nextIndices2[0] && nextIndices1[1] != nextIndices2[1]){
        		// Unique subsequences adding new unique subsequence.
        		printf("Adding branch (%d, %d) splitting points: %d, %d / %d, %d, Branch prefix = %s\n",i,j,nextIndices1[0], nextIndices1[1], nextIndices2[0], nextIndices2[1], subsequence); // Debug statement
        		// PREVIOUS SOLUTION (No uniqueness check while returning but possibility of missing some unique subsequences)
        		// findSubsequences(DPMatrix, str1, str2, nextIndices1[0], nextIndices1[1], subsequence, subseqCount, subsequenceArray); // Branch 1 (current branch)
				// findSubsequences(DPMatrix, str1, str2, i, j-1, nextIndices2[0], nextIndices2[1], subsequenceArray); // Branch 2 (new branch)	
        		findSubsequences(DPMatrix, str1, str2, i-1, j, subsequence, subseqCount, subsequenceArray); // Branch 1 (current branch)
				findSubsequences(DPMatrix, str1, str2, i, j-1, subsequence, subseqCount, subsequenceArray); // Branch 2 (new branch)	
			} else {
				// Both branches converge in same point we are only keeping Branch 1 which is our current subsequence.
				printf("Removing branch %d, %d is converging at: %d, %d\n",i,j,nextIndices1[0], nextIndices1[1], nextIndices2[0], nextIndices2[1]); // Debug statement
				findSubsequences(DPMatrix, str1, str2, i-1, j, subsequence, subseqCount, subsequenceArray); // Branch 1 (current branch)	
			}
			
		  // There are no possible branches check which cell (above or left) is bigger and continue traversing DP matrix from that point.
		} else if(DPMatrix[i-1][j] > DPMatrix[i][j-1]){
			findSubsequences(DPMatrix, str1, str2, i-1, j, subsequence, subseqCount, subsequenceArray);
			
		} else {
			findSubsequences(DPMatrix, str1, str2, i, j-1, subsequence, subseqCount, subsequenceArray);
		}
	}




int main(int argc, char *argv[]) {
	
	int i,j;
	
	char str1[100];
	char str2[100];
	
	printf("Input string 1: ");
	scanf("%s", str1);
	printf("\n");
	printf("Input string 2: ");
	scanf("%s", str2);
	printf("\n");
	
	
	printf("String 1: %s\n", &str1);
	printf("String 2: %s\n\n", &str2);
	
	// Construct DP Matrix from given two strings.
	int** DPMatrix = getDPMatrix(str1, str2);
	printf("====DP MATRIX====\n\n");
	printDPMatrix(DPMatrix,str1, str2);
	int maxSequenceLength = DPMatrix[strlen(str1)][strlen(str2)];
	printf("Max sequence length = %d\n\n",maxSequenceLength);
	
	char subsequence[] = "";
	int subseqCount = 0;
	SubsequenceArray subsequenceArray;
    subsequenceArray.count = 0;
    subsequenceArray.subsequences = malloc(1 * sizeof(char*));
    
    // Find all unique subsequences
    findSubsequences(DPMatrix, str1, str2, strlen(str1), strlen(str2), subsequence, &subseqCount, &subsequenceArray);
    
    printf("\n\n==== SUBSEQUENCES ====\n\n");
	
	printf("Sequence count = %d\n\n", subseqCount);
	
	// Print all unique subsequences
	for(i=0; i<subseqCount; i++){
		printf("Subsequence %d = %s\n",i+1,subsequenceArray.subsequences[i]);
	}
	
	for (i = 0; i < subsequenceArray.count; i++) {
        free(subsequenceArray.subsequences[i]);
    }
    free(subsequenceArray.subsequences);
	
	for (i = 0; i < strlen(str1); i++) {
        free(DPMatrix[i]);
    }
    
    free(DPMatrix);

	
	
	
	return 0;
}
