// Copyright 2011-2012 Jean Charles Régin
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include "base/hash.h"
#include "base/map-util.h"
#include "base/stl_util.h"
#include "base/random.h"
#include "constraint_solver/constraint_solveri.h"
#include "constraint_solver/constraint_solver.h"

DEFINE_int32(arity, 3, "Arity of tuples");
DEFINE_int32(upper, 10, "Upper bound of variables, lower is always 0");
DEFINE_int32(tuples, 1000, "Number of tuples");
DEFINE_int32(bucket, 64, "Size of buckets");

namespace operations_research {
// Enums.
#define TABLECT_RESTART 0
#define TABLECT_CONTINUE 1
#define TABLECT_INVERSE 2
#define TABLECT_ORIGINAL 3
// External build functions.
extern Constraint* BuildTableCt(Solver* const solver,
                                const IntTupleSet& tuples,
                                const std::vector<IntVar*>& vars,
                                int size_bucket);

void RandomFillTable(int num_tuples,
                     int64 lower,
                     int64 upper,
                     IntTupleSet* const tuples) {
  ACMRandom rgen(0); // defines a random generator
  std::vector<int64> vals(tuples->Arity());

  for(int t = 0; t < num_tuples; ++t){
    for(int i = 0; i < tuples->Arity(); i++){
      int64 rnumber = rgen.Next64(); // generates a 64bits number
      vals[i] = (rnumber % (upper - lower + 1)) + lower;
    }
    tuples->Insert(vals);
  }
}

void test_table_in_bk(int n,
                      unsigned num_tuples,
                      unsigned upper,
                      unsigned size_bucket,
                      bool use_bucket_table) {
  // var array creation
  Solver solver("SolverInBk");
  std::vector<IntVar*> vars;
  solver.MakeIntVarArray(n, 0, upper, &vars);

  IntTupleSet table(n);

  if (use_bucket_table) {
    LOG(INFO) <<  "Creation of a Bucketed tuple Table ("
              << size_bucket << ") with :";
  } else {
    LOG(INFO) << "Creation of a Allowed Assignment Table with :";
  }
  LOG(INFO) << " - " << n <<  " variables";
  LOG(INFO) << " - " << upper + 1 <<  " values per domain";
  LOG(INFO) << " - " << num_tuples << " tuples";

  RandomFillTable(num_tuples, 0, upper, &table);

  LOG(INFO) << "Table is created";

  Constraint* const ct = use_bucket_table ?
      BuildTableCt(&solver, table, vars, size_bucket) :
      solver.MakeAllowedAssignments(vars, table);
  solver.AddConstraint(ct);

  LOG(INFO) << "The constraint has been added";

  DecisionBuilder* const db = solver.MakePhase(vars,
                                               Solver::CHOOSE_FIRST_UNBOUND,
                                               Solver::ASSIGN_MIN_VALUE);

  solver.NewSearch(db);

  CycleTimer t;
  t.Start();
  int counter = 0;
  while(solver.NextSolution()) {
    counter++;
  }
  t.Stop();
  solver.EndSearch();
  LOG(INFO) << "test time : " << t.GetInUsec() << " micro seconds";
  CHECK_EQ(counter, table.NumTuples());
}
}  // namespace operations_research


int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  operations_research::test_table_in_bk(FLAGS_arity,
                                        FLAGS_tuples,
                                        FLAGS_upper,
                                        FLAGS_bucket,
                                        false);

  operations_research::test_table_in_bk(FLAGS_arity,
                                        FLAGS_tuples,
                                        FLAGS_upper,
                                        FLAGS_bucket,
                                        true);
  return 0;
}

