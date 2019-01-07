#include "IdealInductor.hh"
#include <cmath>
IdealInductor::IdealInductor( NodeKeeper *nk, const char *name,
          const char *n1,
          const char *n2) : InstanceModel(nk, name)
{
       node_ptr_vtop= this->AddCircuitNode(n1);
       node_ptr_vbot= this->AddCircuitNode(n2);
       node_ptr_I= this->AddInternalNode("I");

       //Parameter List
       L               = 1.000000e+00;
}

void IdealInductor::assembleDC_impl(const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec<double> &mat, std::vector<std::pair<int, double> > &rhs)
{
   const size_t node_num_I = node_ptr_I->getNumber();
   const size_t node_num_vbot = node_ptr_vbot->getNumber();
   const size_t node_num_vtop = node_ptr_vtop->getNumber();

   const bool   is_gnd_node_I = node_ptr_I->isGROUND();
   const bool   is_gnd_node_vbot = node_ptr_vbot->isGROUND();
   const bool   is_gnd_node_vtop = node_ptr_vtop->isGROUND();

   const double I = (is_gnd_node_I) ? 0.0 : sol[node_num_I];
   const double vbot = (is_gnd_node_vbot) ? 0.0 : sol[node_num_vbot];
   const double vtop = (is_gnd_node_vtop) ? 0.0 : sol[node_num_vtop];

   const double vdiff = (vbot - vtop);
   const double eI = vdiff;
   const double evbot = (-I);
   const double evtop = I;

   if (!is_gnd_node_I)
      rhs.push_back(std::make_pair(node_num_I, eI));
   if (!is_gnd_node_vbot)
      rhs.push_back(std::make_pair(node_num_vbot, evbot));
   if (!is_gnd_node_vtop)
      rhs.push_back(std::make_pair(node_num_vtop, evtop));


   const double d_vdiff_d_vtop = (-1);
   const double eI_vtop = d_vdiff_d_vtop;
   const double d_vdiff_d_vbot = 1;
   const double eI_vbot = d_vdiff_d_vbot;
   const double evbot_I = (-1);
   const double evtop_I = 1;

   if (!is_gnd_node_I)
   {
      if (!is_gnd_node_vtop)
         mat.push_back(dsMath::RealRowColVal<double>(node_num_I,node_num_vtop, eI_vtop));
      if (!is_gnd_node_vbot)
         mat.push_back(dsMath::RealRowColVal<double>(node_num_I,node_num_vbot, eI_vbot));
   }
   if (!is_gnd_node_vbot)
   {
      if (!is_gnd_node_I)
         mat.push_back(dsMath::RealRowColVal<double>(node_num_vbot,node_num_I, evbot_I));
   }
   if (!is_gnd_node_vtop)
   {
      if (!is_gnd_node_I)
         mat.push_back(dsMath::RealRowColVal<double>(node_num_vtop,node_num_I, evtop_I));
   }

}


void IdealInductor::assembleTran_impl(const double scl, const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec<double> *mat, std::vector<std::pair<int, double> > &rhs)
{
   const size_t node_num_I = node_ptr_I->getNumber();

   const bool   is_gnd_node_I = node_ptr_I->isGROUND();

   const double I = (is_gnd_node_I) ? 0.0 : sol[node_num_I];

   const double vl = (I * L);
   const double eI = scl *vl;

   if (!is_gnd_node_I)
      rhs.push_back(std::make_pair(node_num_I, eI));


   if (mat == nullptr)
      return;

   const double d_vl_d_I = L;
   const double eI_I = scl * d_vl_d_I;

   if (!is_gnd_node_I)
   {
      mat->push_back(dsMath::RealRowColVal<double>(node_num_I,node_num_I, eI_I));
   }

}

bool IdealInductor::addParam(const std::string &str, double val)
{
    bool ret = false;
    if (str == "L")
    {
        L = val;
        ret = true;
    }
    return ret;
}

extern "C" InstanceModel *IdealInductor_create (NodeKeeper *nk, const std::string &name, const std::vector<std::string> &nodelist) {
 return new IdealInductor(nk, name.c_str(), nodelist[0].c_str(), nodelist[1].c_str());

}
