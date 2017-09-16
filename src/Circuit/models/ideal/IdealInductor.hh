#include "InstanceModel.hh"
#include "CircuitNode.hh"
#include "NodeKeeper.hh"
#include "MatrixEntries.hh"
#include <cmath>

class IdealInductor;
extern "C" InstanceModel *IdealInductor_create (NodeKeeper *, const std::string &name, const std::vector<std::string> &nodelist);
class IdealInductor : public InstanceModel {
   public:
        IdealInductor(
          NodeKeeper *nk,
          const char *name,
          const char *n1,
          const char *n2);

       void assembleDC_impl(const NodeKeeper::Solution &, dsMath::RealRowColValueVec<double> &, std::vector<std::pair<int, double> > &);
       void assembleTran_impl(const double scl, const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec<double> *mat, std::vector<std::pair<int, double> > &rhs);
       bool addParam(const std::string &, double);
    private:
       IdealInductor();
       IdealInductor(const IdealInductor &);
       IdealInductor operator=(const IdealInductor &);

       //External Nodes
       CircuitNodePtr node_ptr_vtop;
       CircuitNodePtr node_ptr_vbot;

       //Internal Nodes
       CircuitNodePtr node_ptr_I;

       //Parameter List
       double L;             //"Inductance"         1.000000e+00
};
