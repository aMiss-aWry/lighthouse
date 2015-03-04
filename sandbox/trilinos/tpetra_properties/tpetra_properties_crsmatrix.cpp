#include "tpetra_properties_crsmatrix.h"

RCP<const Teuchos::Comm<int> > comm;
RCP<Teuchos::FancyOStream> fos;
int numNodes;

int main(int argc, char *argv[]) {
	
	//  General setup for Teuchos/communication
	Teuchos::GlobalMPISession mpiSession(&argc, &argv);
	Platform& platform = Tpetra::DefaultPlatform::getDefaultPlatform();
	comm = rcp (new Teuchos::MpiComm<int> (MPI_COMM_WORLD));	
	RCP<NT> node = platform.getNode();
	int myRank = comm->getRank();
	Teuchos::oblackholestream blackhole;
	std::ostream& out = (myRank == 0) ? std::cout : blackhole;
	fos = Teuchos::fancyOStream(Teuchos::rcpFromRef(out));

	// Load and run tests on Matrix Market file
	std::string filename("../bp_1200.mtx");
	RCP<MAT> A = Reader::readSparseFile(filename, comm, node, true);
	runGauntlet(A);
	calcLambdaMaxByMagnitudeReal(A, argc, argv);
}

void runGauntlet(const RCP<MAT> &A) {
	// Test squareness
	if (A->getGlobalNumRows() != A->getGlobalNumCols() ) {
		*fos << "Not a square matrix, exiting\n";
		exit(-1);
	}
	numNodes = comm->getSize();
	*fos << "nodes:" << numNodes << " nodes" << std::endl;

// Working
	calcRowVariance(A); // ecl32, 
	calcColVariance(A); // ecl32
	calcDiagVariance(A);
	calcNonzeros(A);
	calcDim(A);
	calcFrobeniusNorm(A);
	calcSymmetricFrobeniusNorm(A);
	calcAntisymmetricFrobeniusNorm(A);
	calcOneNorm(A);
	calcInfNorm(A);
	calcSymmetricInfNorm(A);
	calcAntisymmetricInfNorm(A);
	calcMaxNonzerosPerRow(A);
	calcMinNonzerosPerRow(A);
	calcAvgNonzerosPerRow(A);
	calcTrace(A);
	calcAbsTrace(A);
	calcDummyRows(A);	
	//calcNumericalSymmetryPercentage(A); //Single process only
	//calcNonzeroPatternSymmetryPercentage(A); //Single process only
	calcRowDiagonalDominance(A);
	calcColDiagonalDominance(A);
	calcLowerBandwidth(A);
	calcUpperBandwidth(A);
	calcDiagonalSign(A);
	calcDiagonalNonzeros(A);
}

//  Return the maximum row locVariance for the matrix
//  The average of the squared differences from the Mean.
void calcRowVariance(const RCP<MAT> &A) {
	LO rows = A->getGlobalNumRows(); 
	ST mean, locVariance, locMaxVariance, result = 0.0;

	//  Go through each row on the current process
	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			mean = locVariance = 0.0; 
			size_t cols = A->getNumEntriesInGlobalRow(row); 
			Array<ST> values(cols);
			Array<GO> indices(cols);
			A->getGlobalRowCopy(row, indices(), values(), cols); 

		//  Two-step approach for locVariance, could be more efficient 
			for (LO col = 0; col < cols; col++) {
				mean += values[col];
			} 
		//  Divide entries by the dim (to include zeros)
			mean /= A->getGlobalNumCols();
			for (LO col = 0; col < cols; col++) {
				locVariance += (values[col] - mean) * (values[col] - mean);
			}
			locVariance /= A->getGlobalNumCols();
			if (locVariance > locMaxVariance) locMaxVariance = locVariance;
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_MAX, 1, &locMaxVariance, &result);
	*fos << "row variance:" << result << std::endl;
}

//  Transpose the matrix, get row locVariance 
void calcColVariance(const RCP<MAT> &A) {
	Tpetra::RowMatrixTransposer<ST, LO, GO, NT> transposer(A);	
	RCP<MAT> B = transposer.createTranspose();
	
	LO rows = B->getGlobalNumRows(); 
	ST mean, locVariance, locMaxVariance, result = 0.0;

	//  Go through each row on the current process
	for (GO row = 0; row < rows; row++) {
		if (B->getRowMap()->isNodeGlobalElement(row)) {
			mean = locVariance = 0.0; 
			size_t cols = B->getNumEntriesInGlobalRow(row); 
			Array<ST> values(cols);
			Array<GO> indices(cols);
			B->getGlobalRowCopy(row, indices(), values(), cols); 

		//  Two-step approach for locVariance, could be more efficient 
			for (LO col = 0; col < cols; col++) {
				mean += values[col];
			} 
		//  Divide entries by the dim (to include zeros)
			mean /= B->getGlobalNumCols();
			for (LO col = 0; col < cols; col++) {
				locVariance += (values[col] - mean) * (values[col] - mean);
			}
			locVariance /= B->getGlobalNumCols();
			if (locVariance > locMaxVariance) locMaxVariance = locVariance;
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_MAX, 1, &locMaxVariance, &result);
	*fos << "col variance:" << result << std::endl;
}

//  The variance of the diagonal
void calcDiagVariance(const RCP<MAT> &A) {
	LO rows = A->getGlobalNumRows(); 
	ST locMean = 0.0;
	ST mean = 0.0, locVariance = 0.0, result = 0.0;

	//  Go through each row on the current process
	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			size_t cols = A->getNumEntriesInGlobalRow(row);
			Array<ST> values(cols);
			Array<GO> indices(cols);
			A->getGlobalRowCopy(row, indices(), values(), cols);
			for (size_t col = 0; col < cols; col++) {
				if (indices[col] == row) {
					locMean += values[col]; 
				}
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_SUM, 1, &locMean, &mean);
	mean /= A->getGlobalNumRows();
	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			size_t cols = A->getNumEntriesInGlobalRow(row);
			Array<ST> values(cols);
			Array<GO> indices(cols);
			A->getGlobalRowCopy(row, indices(), values(), cols);
			for (size_t col = 0; col < cols; col++) {
				if (indices[col] == row) {
					locVariance += (values[col] - mean) * (values[col] - mean);
				}
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_SUM, 1, &locVariance, &result);
	result /= A->getGlobalNumRows();
	*fos << "diag variance:" << result << std::endl;
}

//  Total number of nonzeros in matrix
void calcNonzeros(const RCP<MAT> &A) {
	*fos << "nonzeros:" << A->getGlobalNumEntries() << ", " << std::endl;
}

//  Dimension of the square matrix
void calcDim(const RCP<MAT> &A) {
	*fos << "dimension:" << A->getGlobalNumRows() << ", " << std::endl;
}

//  Frobenius norm of matrix
void calcFrobeniusNorm(const RCP<MAT> &A) {
	*fos << "frob norm:" << A->getFrobeniusNorm() << ", " << std::endl;
}

//  Symmetric A_s = (A+A')/2
void calcSymmetricFrobeniusNorm(const RCP<MAT> &A){ 
	RCP<MAT> A_s = Tpetra::MatrixMatrix::add(0.5, false, *A, 0.5, true, *A);
	*fos << "symm frob norm:" << A_s->getFrobeniusNorm() << ", " << std::endl;
}

//  Antisymmetric A_a = (A-A')/2
void calcAntisymmetricFrobeniusNorm(const RCP<MAT> &A){ 
	RCP<MAT> A_a = Tpetra::MatrixMatrix::add(0.5, false, *A, -0.5, true, *A);
	*fos << "antisymm frob norm:" << A_a->getFrobeniusNorm() << ", " << std::endl;
}

//  Max absolute row sum
void calcInfNorm(const RCP<MAT> &A) {
	GO rows = A->getGlobalNumRows(); 
	ST locSum, locMaxSum, result = 0.0;
	//  Go through each row on the current process
	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			locSum = 0;
			size_t cols = A->getNumEntriesInGlobalRow(row);
			Array<ST> values(cols);
			Array<GO> indices(cols);
			A->getGlobalRowCopy(row, indices(), values(), cols); 
			for (LO col = 0; col < cols; col++) {
				locSum += fabs(values[col]);
			} 
			if (locSum > locMaxSum) {
				locMaxSum = locSum;
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_MAX, 1, &locMaxSum, &result);
	*fos << "inf norm:" << result << std::endl;
}

//  Max absolute column sum
void calcOneNorm(const RCP<MAT> &A) {
	Tpetra::RowMatrixTransposer<ST, LO, GO, NT> transposer(A);	
	RCP<MAT> B = transposer.createTranspose();

	GO rows = B->getGlobalNumRows(); 
	ST locSum, locMaxSum, result = 0.0;
	//  Go through each row on the current process
	for (GO row = 0; row < rows; row++) {
		if (B->getRowMap()->isNodeGlobalElement(row)) {
			locSum = 0;
			size_t cols = B->getNumEntriesInGlobalRow(row);
			Array<ST> values(cols);
			Array<GO> indices(cols);
			B->getGlobalRowCopy(row, indices(), values(), cols); 
			for (LO col = 0; col < cols; col++) {
				locSum += fabs(values[col]);
			} 
			if (locSum > locMaxSum) {
				locMaxSum = locSum;
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_MAX, 1, &locMaxSum, &result);
	*fos << "one norm:" << result << std::endl;
}

//  Max absolute row sum of symmetric part
void calcSymmetricInfNorm(const RCP<MAT> &A) {
	RCP<MAT> A_s = Tpetra::MatrixMatrix::add(0.5, false, *A, 0.5, true, *A);
	*fos << "symmetric ";
	calcInfNorm(A_s);
}

//  Max absolute row sum of anti-symmetric part
void calcAntisymmetricInfNorm(const RCP<MAT> &A) {
	RCP<MAT> A_a = Tpetra::MatrixMatrix::add(0.5, false, *A, -0.5, true, *A);
	*fos << "anti-symmetric ";
	calcInfNorm(A_a);
}

//  Self explanatory
void calcMaxNonzerosPerRow(const RCP<MAT> &A) {
	size_t result = A->getGlobalMaxNumRowEntries();	
	*fos << "max nonzeros per row:" << result << std::endl;
}

void calcMinNonzerosPerRow(const RCP<MAT> &A) {
	GO rows = A->getGlobalNumRows();
	GO locNonzeros = rows, locMinNonzeros = rows, result = 0;	

	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			locNonzeros = A->getNumEntriesInGlobalRow(row);
			if (locNonzeros >= 0) {
				if (locNonzeros < locMinNonzeros) {
					locMinNonzeros = locNonzeros;
				}
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_MIN, 1, &locMinNonzeros, &result);
	*fos << "min nonzeros per row:" << result << std::endl;
}

void calcAvgNonzerosPerRow(const RCP<MAT> &A) {
	GO rows = A->getGlobalNumRows();
	GO locNonzeros = 0, result = 0;

	//  Go through each row on the current process
	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			if (A->getNumEntriesInGlobalRow(row) >= 0) {
				locNonzeros += A->getNumEntriesInGlobalRow(row);	
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_SUM, 1, &locNonzeros, &result);
	double castResult = (double)result / (double)rows;
	*fos << "avg nonzeros per row:" << castResult << std::endl;	
}

void calcTrace(const RCP<MAT> &A) {
	LO rows = A->getGlobalNumRows(); 
	ST trace = 0.0, result = 0.0;

	//  Go through each row on the current process
	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			size_t cols = A->getNumEntriesInGlobalRow(row);
			Array<ST> values(cols);
			Array<GO> indices(cols);
			A->getGlobalRowCopy(row, indices(), values(), cols);
			for (size_t col = 0; col < cols; col++) {
				if (indices[col] == row) {
						trace += values[col]; 
				}
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_SUM, 1, &trace, &result);
	*fos << "trace:" << result << std::endl;
}

void calcAbsTrace(const RCP<MAT> &A) {
	LO rows = A->getGlobalNumRows(); 
	ST trace = 0.0, result = 0.0;

	//  Go through each row on the current process
	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			size_t cols = A->getNumEntriesInGlobalRow(row);
			Array<ST> values(cols);
			Array<GO> indices(cols);
			A->getGlobalRowCopy(row, indices(), values(), cols);
			for (size_t col = 0; col < cols; col++) {
				if (indices[col] == row) {
					trace += fabs(values[col]);
				}
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_SUM, 1, &trace, &result);
	*fos << "trace:" << result << std::endl;	
}

void calcDummyRows(const RCP<MAT> &A) {
	LO rows = A->getGlobalNumRows(); 
	GO locDummy = 0, result = 0;

	//  Go through each row on the current process
	for (LO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			if (A->getNumEntriesInGlobalRow(row) == 1) {
				locDummy++;
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_SUM, 1, &locDummy, &result);
	*fos << "dummy rows:" << result << std::endl;
}

void calcNumericalSymmetryPercentageMPI(const RCP<MAT> &A) {
	Tpetra::RowMatrixTransposer<ST, LO, GO, NT> transposer(A);	
	RCP<MAT> B = transposer.createTranspose();

	Array<ST> valuesA, valuesB;
	Array<GO> indicesA, indicesB;
	int test;
	GO rows = A->getGlobalNumRows();
	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			size_t colsA = A->getNumEntriesInGlobalRow(row);
			valuesA = Array<ST>(colsA);
			indicesA = Array<GO>(colsA);
			A->getGlobalRowCopy(row, indicesA(), valuesA(), colsA);
			Teuchos::broadcast<int, ST>(*comm, comm->getRank(), valuesA);
		}
		/*
		if (B->getRowMap()->isNodeGlobalElement(row)) {
			size_t colsB = B->getNumEntriesInGlobalRow(row);
			valuesB = Array<ST>(colsB);
			indicesB = Array<GO>(colsB);
		}
		*/
		if (row == 4176) {
			std::cout << "rank:" << comm->getRank() << "," << row << "\t" << valuesA << std::endl;
			exit;
		}
	}
}

//  Exact match
void calcNumericalSymmetryPercentage(const RCP<MAT> &A) {
	Tpetra::RowMatrixTransposer<ST, LO, GO, NT> transposer(A);	
	RCP<MAT> B = transposer.createTranspose();

	GO rows = A->getGlobalNumRows(); 
	ST result = 0.0;
	GO totalMatch, match = 0;
	GO locEntries = 0;

	GO diagNonzeros = A->getGlobalNumDiags();
	GO offDiagNonzeros = A->getGlobalNumEntries() - diagNonzeros;
	for (GO row = 0; row < rows; row++) {
		size_t colsA = A->getNumEntriesInGlobalRow(row);
		size_t colsB = B->getNumEntriesInGlobalRow(row);
		Array<ST> valuesA(colsA), valuesB(colsB);
		Array<GO> indicesA(colsA), indicesB(colsB);
		A->getGlobalRowCopy(row, indicesA(), valuesA(), colsA); 
		B->getGlobalRowCopy(row, indicesB(), valuesB(), colsB);
		size_t i = 0, j = 0;
		if (colsA < A->getGlobalNumRows() && colsB < A->getGlobalNumRows()) {
			while (i < colsA && j < colsB) {
				if (indicesA[i] < indicesB[j]) {
					i++;
				} else if (indicesA[i] > indicesB[j]) {
					j++;
				} else {
					if (valuesA[i] == valuesB[j] && row != indicesA[i]) {
						match++;
					}
					i++; j++;
				}
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_SUM, 1, &match, &totalMatch);
	result = double(totalMatch) / double(offDiagNonzeros);
	*fos << "numerical symmetry percentage:" << result << std::endl;
	if (result < 1.0) {
		*fos << "numerical symmetry:0" << std::endl;
	} else {
		*fos << "numerical symmetry:1" << std::endl;
	}
}

void calcNonzeroPatternSymmetryPercentage(const RCP<MAT> &A) {
	Tpetra::RowMatrixTransposer<ST, LO, GO, NT> transposer(A);	
	RCP<MAT> B = transposer.createTranspose();

	GO rows = A->getGlobalNumRows(); 
	ST result = 0.0;
	GO totalMatch, match = 0;
	GO locEntries = 0;

	GO diagNonzeros = A->getGlobalNumDiags();
	GO offDiagNonzeros = A->getGlobalNumEntries() - diagNonzeros;
	for (GO row = 0; row < rows; row++) {
		size_t colsA = A->getNumEntriesInGlobalRow(row);
		size_t colsB = B->getNumEntriesInGlobalRow(row);
		Array<ST> valuesA(colsA), valuesB(colsB);
		Array<GO> indicesA(colsA), indicesB(colsB);
		A->getGlobalRowCopy(row, indicesA(), valuesA(), colsA); 
		B->getGlobalRowCopy(row, indicesB(), valuesB(), colsB);
		size_t i = 0, j = 0;
		if (colsA < A->getGlobalNumRows() && colsB < A->getGlobalNumRows()) {
			while (i < colsA && j < colsB) {
				if (indicesA[i] < indicesB[j]) {
					i++;
				} else if (indicesA[i] > indicesB[j]) {
					j++;
				} else {
					if (valuesA[i] && valuesB[j] && row != indicesA[i]) {
						match++;
					}
					i++; j++;
				}
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_SUM, 1, &match, &totalMatch);
	result = double(totalMatch) / double(offDiagNonzeros);
	*fos << "nonzero symmetry percentage:" << result << std::endl;
	if (result < 1.0) {
		*fos << "nonzero symmetry:0" << std::endl;
	} else {
		*fos << "nonzero symmetry:1" << std::endl;
	}
}

// 0 not, 1 weak, 2 strict
// a_ii >= sum(a_ij) for all i,js i!=j
void calcRowDiagonalDominance(const RCP<MAT> &A) {
	GO rows = A->getGlobalNumRows(); 
	ST result = 0.0;
	GO totalMatch, match = 0;
	GO locEntries = 0;
	ST locDiagSum = 0.0, locRowSum = 0.0;
	ST totalDiagSum, totalRowSum;
	int strict = 1, totalStrict;

	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			size_t cols = A->getNumEntriesInGlobalRow(row);
			Array<ST> values(cols);
			Array<GO> indices(cols);
			A->getGlobalRowCopy(row, indices(), values(), cols); 
			size_t i = 0, j = 0;
			if (cols < A->getGlobalNumRows()) {
				totalDiagSum = totalRowSum = 0.0;
				for (size_t col = 0; col < cols; col++) {
					if (row == indices[col]) {
						totalDiagSum += values[col];
					} else {
						totalRowSum += values[col];
					}
				}
				if (locDiagSum < locRowSum) {
					*fos << "row diagonal dominance:0" << std::endl;
					return;
				} else if (locDiagSum == locRowSum) {
					strict = 0;
				}
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_MAX, 1, &strict, &totalStrict);
	if (totalStrict == 1) {
		*fos << "row diagonal dominance:2" << std::endl;
	} else {
		*fos << "row diagonal dominance:1" << std::endl;
	}
}

void calcColDiagonalDominance(const RCP<MAT> &A) {
	Tpetra::RowMatrixTransposer<ST, LO, GO, NT> transposer(A);	
	RCP<MAT> B = transposer.createTranspose();

	GO rows = B->getGlobalNumRows(); 
	ST result = 0.0;
	GO totalMatch, match = 0;
	GO locEntries = 0;
	ST locDiagSum = 0.0, locRowSum = 0.0;
	ST totalDiagSum, totalRowSum;
	int strict = 1, totalStrict;

	for (GO row = 0; row < rows; row++) {
		if (B->getRowMap()->isNodeGlobalElement(row)) {
			size_t cols = B->getNumEntriesInGlobalRow(row);
			Array<ST> values(cols);
			Array<GO> indices(cols);
			B->getGlobalRowCopy(row, indices(), values(), cols); 
			size_t i = 0, j = 0;
			if (cols < B->getGlobalNumRows()) {
				totalDiagSum = totalRowSum = 0.0;
				for (size_t col = 0; col < cols; col++) {
					if (row == indices[col]) {
						totalDiagSum += values[col];
					} else {
						totalRowSum += values[col];
					}
				}
				if (locDiagSum < locRowSum) {
					*fos << "col diagonal dominance:0" << std::endl;
					return;
				} else if (locDiagSum == locRowSum) {
					strict = 0;
				}
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_MAX, 1, &strict, &totalStrict);
	if (totalStrict == 1) {
		*fos << "col diagonal dominance:2" << std::endl;
	} else {
		*fos << "col diagonal dominance:1" << std::endl;
	}	
}

void calcDiagonalMean(const RCP<MAT> &A) {
	ST locMean, mean = 0.0;
	GO numGlobalElements = A->getGlobalNumDiags(); 
	RCP<const MAP> map = rcp(new MAP (numGlobalElements, 0, comm)); 
	VEC v(map);
	A->getLocalDiagCopy(v);
	Teuchos::ArrayRCP<const ST> array = v.getData();	
	for (int i = 0; i < array.size(); i++) {
		locMean += array[i];	
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_SUM, 1, &locMean, &mean);
	mean /= A->getGlobalNumRows();
	*fos << "diag mean" << mean << ", " << std::endl;
}

// indicates the diagonal sign pattern
// -2 all negative, -1 nonpositive, 0 all zero, 1 nonnegative, 2 all positive, 
// 3 some negative,some or no zero,some positive
void calcDiagonalSign(const RCP<MAT> &A) {
	long locPos = 0, locNeg = 0, locZero = 0;
	long totalPos, totalNeg, totalZero;
	GO rows = A->getGlobalNumRows();
	
	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			size_t cols = A->getNumEntriesInGlobalRow(row);
			Array<ST> values(cols);
			Array<GO> indices(cols);
			A->getGlobalRowCopy(row, indices(), values(), cols);
			for (size_t col = 0; col < cols; col++) {
				if (indices[col] == row) {
					if (values[col] > 0) {
						locPos++;
					} else if (values[col] < 0) {
						locNeg++;
					} else {
						locZero++;
					}
				}
			}
		}
	}

	Teuchos::reduceAll(*comm, Teuchos::REDUCE_SUM, 1, &locPos, &totalPos);
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_SUM, 1, &locNeg, &totalNeg);
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_SUM, 1, &locZero, &totalZero);
	*fos << "diagonal sign:";
	if (totalPos > 0 && totalNeg == 0 && totalZero == 0) {
		*fos << "2" << std::endl;
	} else if (totalNeg > 0 && totalPos == 0 && totalZero == 0) {
		*fos << "-2" << std::endl;
	} else if (totalZero > 0 && totalPos == 0 && totalNeg == 0) {
		*fos << "0" << std::endl;
	} else if (totalNeg == 0) {
		*fos << "1" << std::endl;
	} else if (totalPos == 0) {
		*fos << "-1" << std::endl;
	} else {
		*fos << "3" << std::endl;
	}
}

void calcDiagonalNonzeros(const RCP<MAT> &A) {
	GO diagNonzeros = A->getGlobalNumDiags();
	*fos << "nonzeros on diag:" << diagNonzeros << std::endl;
}

void calcLowerBandwidth(const RCP<MAT> &A) {
	LO rows = A->getGlobalNumRows();
	GO localMaxLB = 0, localLB = 0, totalLB;
	GO minIndex;

	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			size_t cols = A->getNumEntriesInGlobalRow(row);
			if (cols > 0 && cols <= A->getGlobalNumRows()) {
				Array<ST> values(cols);
				Array<GO> indices(cols);
				A->getGlobalRowCopy(row, indices(), values(), cols); 
				minIndex = indices[0];
				for (size_t col = 1; col < cols; col++) {
					if (indices[col] < minIndex) {
						minIndex = indices[col];
					}	
				}
				localLB = row - minIndex;
				if (localLB > localMaxLB) {
					localMaxLB = localLB;
				}
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_MAX, 1, &localMaxLB, &totalLB);
	*fos << "lb:" << totalLB << std::endl;
}

void calcUpperBandwidth(const RCP<MAT> &A) {
	LO rows = A->getGlobalNumRows();
	GO localMaxUB = 0, localUB = 0, totalUB;
	GO maxIndex;

	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			size_t cols = A->getNumEntriesInGlobalRow(row);
			if (cols > 0 && cols <= A->getGlobalNumRows()) {
				Array<ST> values(cols);
				Array<GO> indices(cols);
				A->getGlobalRowCopy(row, indices(), values(), cols); 
				maxIndex = indices[0];
				for (size_t col = 1; col < cols; col++) {
					if (indices[col] > maxIndex) {
						maxIndex = indices[col];
					}	
				}
				localUB = maxIndex - row;
				if (localUB > localMaxUB) {
					localMaxUB = localUB;
				}
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_MAX, 1, &localMaxUB, &totalUB);
	*fos << "ub:" << totalUB << std::endl;
}

void calcBandwidth(const RCP<MAT> &A) {
  LO rows = A->getGlobalNumRows();
	GO localMaxUB = 0, localUB = 0, totalUB;
	GO localMaxLB = 0, localLB = 0, totalLB;
	GO maxIndex, minIndex;

	for (GO row = 0; row < rows; row++) {
		if (A->getRowMap()->isNodeGlobalElement(row)) {
			size_t cols = A->getNumEntriesInGlobalRow(row);
			if (cols > 0 && cols <= A->getGlobalNumRows()) {
				Array<ST> values(cols);
				Array<GO> indices(cols);
				A->getGlobalRowCopy(row, indices(), values(), cols); 
				minIndex = maxIndex = indices[0];
				for (size_t col = 1; col < cols; col++) {
					if (indices[col] > maxIndex) {
						maxIndex = indices[col];
					}	
					if (indices[col] < minIndex) {
						minIndex = indices[col];
					}
				}
				localUB = maxIndex - row;
				localLB = row - maxIndex;
				if (localUB > localMaxUB) {
					localMaxUB = localUB;
				}
				if (localLB < localMaxLB) {
					localMaxLB = localLB;
				}
			}
		}
	}
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_MAX, 1, &localMaxUB, &totalUB);
	Teuchos::reduceAll(*comm, Teuchos::REDUCE_MAX, 1, &localMaxLB, &totalLB);
	*fos << "ub:" << totalUB << std::endl;
	*fos << "lb:" << totalLB << std::endl;
}

// based off tinyurl.com/ktlpsah
void calcLambdaMaxByMagnitudeReal(const RCP<MAT> &A, int argc, char *argv[]) {
	std::string filenameA ("../bcsstk06.mtx");
  std::string filenameB ("../bcsstm06.mtx");
  ST tol = 1e-6;
  int nev = 4;
  int blockSize = 1;
  bool verbose = true;
  std::string whenToShift = "Always";

	//  Process command line args 
  Teuchos::CommandLineProcessor cmdp(false,true);
  cmdp.setOption("fileA",&filenameA, "Filename for the Matrix-Market matrix.");
  cmdp.setOption("tolerance",&tol, "Relative residual used for solver.");
  cmdp.setOption("nev",&nev, "Number of desired eigenpairs.");
  cmdp.setOption("blocksize",&blockSize, "Number of vectors to add to the subspace at each iteration.");
  cmdp.setOption("verbose","quiet",&verbose, "Whether to print a lot of info or a little bit.");
  cmdp.setOption("whenToShift",&whenToShift, "When to perform Ritz shifts. Options: Never, After Trace Levels, Always.");
  if(cmdp.parse(argc,argv) != Teuchos::CommandLineProcessor::PARSE_SUCCESSFUL) {
  	*fos << "PARSE_UNSUCCESSFUL" << std::endl;
    return;
  }

  //  Read matrices, calculate matrix norm
  Platform& platform = Tpetra::DefaultPlatform::getDefaultPlatform();
  RCP<NT> node = platform.getNode();
  RCP<const MAT> K = Reader::readSparseFile(filenameA, comm, node);
  RCP<const MAT> M = Reader::readSparseFile(filenameB, comm, node);
  ST mat_norm = std::max(K->getFrobeniusNorm(),M->getFrobeniusNorm());	

  // Start the block Arnoldi iteration
  int verbosity;
	int numRestartBlocks = 2*nev/blockSize;
	int numBlocks = 10*nev/blockSize;
	if(verbose) {
    verbosity = Anasazi::TimingDetails + Anasazi::IterationDetails + Anasazi::Debug + Anasazi::FinalSummary;
	} else {
    verbosity = Anasazi::TimingDetails;	
	}

	Teuchos::ParameterList MyPL;
  MyPL.set( "Verbosity", verbosity );                  // How much information should the solver print?
  MyPL.set( "Saddle Solver Type", "Projected Krylov"); // Use projected minres/gmres to solve the saddle point problem
  MyPL.set( "Block Size", blockSize );                 // Add blockSize vectors to the basis per iteration
  MyPL.set( "Convergence Tolerance", tol*mat_norm );   // How small do the residuals have to be
  MyPL.set( "Relative Convergence Tolerance", false);  // Don't scale residuals by eigenvalues (when checking for convergence)
  MyPL.set( "Use Locking", true);                      // Use deflation
  MyPL.set( "Relative Locking Tolerance", false);      // Don't scale residuals by eigenvalues (when checking whether to lock a vector)
  MyPL.set("Num Restart Blocks", numRestartBlocks);    // When we restart, we start back up with 2*nev blocks
  MyPL.set("Num Blocks", numBlocks);                   // Maximum number of blocks in the subspace
  MyPL.set("When To Shift", whenToShift);
}



















