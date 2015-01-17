#include "InstanceModel.hh"
#include "CircuitNode.hh"
#include "NodeKeeper.hh"
#include "MatrixEntries.hh"
#include <cmath>

class diode;
extern "C" InstanceModel *diode_create (NodeKeeper *, const std::string &name, const std::vector<std::string> &nodelist);
class diode : public InstanceModel {
   public:
        diode(
          NodeKeeper *nk,
          const char *name,
          const char *n1,
          const char *n2);

       void assembleDC(const NodeKeeper::Solution &, dsMath::RealRowColValueVec &, std::vector<std::pair<int, double> > &);
       void assembleTran(const double scl, const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec *mat, std::vector<std::pair<int, double> > &rhs);
       bool addParam(const std::string &, double);
    private:
       diode();
       diode(const diode &);
       diode operator=(const diode &);

       //External Nodes
       CircuitNodePtr node_ptr_va;
       CircuitNodePtr node_ptr_vc;

       //Internal Nodes
       CircuitNodePtr node_ptr_vx;

       //Parameter List
       double C;             //"Capacitance"        1.000000e+00
       double Is;            //"sat current"        1.000000e+00
       double R;             //"series R"           1.000000e+00
       double VT;            //"thermal voltage"    1.000000e+00
       double n;             //"ideality"           1.000000e+00
};
