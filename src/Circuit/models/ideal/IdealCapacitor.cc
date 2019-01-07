#include "IdealCapacitor.hh"
#include <cmath>
IdealCapacitor::IdealCapacitor( NodeKeeper *nk, const char *name,
          const char *n1,
          const char *n2) : InstanceModel(nk, name)
{
       node_ptr_vtop= this->AddCircuitNode(n1);
       node_ptr_vbot= this->AddCircuitNode(n2);

       //Parameter List
       C               = 1.000000e+00;
}

void IdealCapacitor::assembleDC_impl(const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec<double> &mat, std::vector<std::pair<int, double> > &rhs)
{








}


void IdealCapacitor::assembleTran_impl(const double scl, const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec<double> *mat, std::vector<std::pair<int, double> > &rhs)
{
   const size_t node_num_vbot = node_ptr_vbot->getNumber();
   const size_t node_num_vtop = node_ptr_vtop->getNumber();

   const bool   is_gnd_node_vbot = node_ptr_vbot->isGROUND();
   const bool   is_gnd_node_vtop = node_ptr_vtop->isGROUND();

   const double vbot = (is_gnd_node_vbot) ? 0.0 : sol[node_num_vbot];
   const double vtop = (is_gnd_node_vtop) ? 0.0 : sol[node_num_vtop];

   const double iq = (C * (vtop - vbot));
   const double evbot = scl *(-iq);
   const double evtop = scl *iq;

   if (!is_gnd_node_vbot)
      rhs.push_back(std::make_pair(node_num_vbot, evbot));
   if (!is_gnd_node_vtop)
      rhs.push_back(std::make_pair(node_num_vtop, evtop));


   if (mat == nullptr)
      return;

   const double d_iq_d_vtop = C;
   const double evbot_vtop = scl * (-d_iq_d_vtop);
   const double d_iq_d_vbot = (-C);
   const double evbot_vbot = scl * (-d_iq_d_vbot);
   const double evtop_vtop = scl * d_iq_d_vtop;
   const double evtop_vbot = scl * d_iq_d_vbot;

   if (!is_gnd_node_vbot)
   {
      if (!is_gnd_node_vtop)
         mat->push_back(dsMath::RealRowColVal<double>(node_num_vbot,node_num_vtop, evbot_vtop));
      mat->push_back(dsMath::RealRowColVal<double>(node_num_vbot,node_num_vbot, evbot_vbot));
   }
   if (!is_gnd_node_vtop)
   {
      mat->push_back(dsMath::RealRowColVal<double>(node_num_vtop,node_num_vtop, evtop_vtop));
      if (!is_gnd_node_vbot)
         mat->push_back(dsMath::RealRowColVal<double>(node_num_vtop,node_num_vbot, evtop_vbot));
   }

}

bool IdealCapacitor::addParam(const std::string &str, double val)
{
    bool ret = false;
    if (str == "C")
    {
        C = val;
        ret = true;
    }
    return ret;
}

extern "C" InstanceModel *IdealCapacitor_create (NodeKeeper *nk, const std::string &name, const std::vector<std::string> &nodelist) {
 return new IdealCapacitor(nk, name.c_str(), nodelist[0].c_str(), nodelist[1].c_str());

}
