//  Tpetra
#include <Tpetra_CrsMatrix.hpp>
#include <MatrixMarket_Tpetra.hpp>

//  Belos
#include <BelosTpetraAdapter.hpp>
#include <BelosSolverFactory.hpp>

//  Ifpack2
#include <Ifpack2_Factory.hpp>

//  c++
#include <exception>

//  Typedefs
typedef double ST;
typedef int LO;
typedef int64_t GO;
typedef Tpetra::DefaultPlatform::DefaultPlatformType Platform;
typedef Tpetra::Map<>::node_type NT;
typedef Tpetra::CrsMatrix<ST, LO, GO, NT> MAT;
typedef Tpetra::MultiVector<ST, LO, GO, NT> MV;
typedef Tpetra::MatrixMarket::Reader<MAT> Reader;
typedef Tpetra::Operator<ST, LO, GO, NT> OP;
typedef Ifpack2::Preconditioner<ST, LO, GO, NT> PRE;
typedef Belos::LinearProblem<ST, MV, OP> LP;
typedef Belos::SolverManager<ST, MV, OP> BSM;
typedef const std::vector<std::string> STRINGS;

//  Namespaces
using Tpetra::global_size_t;
using Tpetra::Map;
using Tpetra::Import;
using Teuchos::RCP;
using Teuchos::rcpFromRef;
using Teuchos::ArrayView;
using Teuchos::Array;
using Teuchos::Time;
using Teuchos::TimeMonitor;
using Teuchos::ParameterList;
using Teuchos::parameterList;

//  Globals
//  5 precs, 14 solvers, 70 combinations
STRINGS ifpack2Precs = {"ILUT", "RILUK", "DIAGONAL", "RELAXATION", "CHEBYSHEV", "None"};

STRINGS belos_all = {"BLOCK GMRES", "PSEUDOBLOCK GMRES", "BLOCK CG",
    "PSEUDOBLOCK CG", "PSEUDOBLOCK STOCHASTIC CG", "GCRODR", "RCG", "MINRES", "TFQMR",
    "PSEUDOBLOCK TFQMR", "HYBRID BLOCK GMRES", "PCPG", "BICGSTAB", "LSQR"};

STRINGS belos_sq  = {"PSEUDOBLOCK TFQMR", "TFQMR", "BICGSTAB", "BLOCK GMRES", 
    "PSEUDOBLOCK GMRES", "HYBRID BLOCK GMRES", "GCRODR", "LSQR"};
STRINGS belos_sym = {"MINRES", "BLOCK CG", "PSEUDOBLOCK CG", "PSEUDOBLOCK STOCHASTIC CG", 
    "RCG",  "PCPG"};
std::vector<std::string> belosSolvers;

//  Functions
STRINGS determineSolvers(const std::string &filename);
void belosSolve(const RCP<const MAT> &A, const std::string &filename);
RCP<PRE> getIfpack2Preconditoner(const RCP<const MAT> &A, std::string ifpack2PrecChoice);

RCP<const Teuchos::Comm<int> > comm;
RCP<Teuchos::FancyOStream> fos;
int myRank;
