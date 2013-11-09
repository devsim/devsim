#include "mctest_user.hh"
#include <cmath>
mctest_user::mctest_user( NodeKeeper *nk, const char *name,
          const char *n1,
          const char *n2) : InstanceModel(nk, name)
{
       node_ptr_va= this->AddCircuitNode(n1);
       node_ptr_vc= this->AddCircuitNode(n2);
       node_ptr_vx= this->AddInternalNode("vx");

       //Parameter List
       C               = 1.000000e+00;
       Is              = 1.000000e+00;
       R               = 1.000000e+00;
       VT              = 1.000000e+00;
       n               = 1.000000e+00;
}

void mctest_user::assembleDC(const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec &mat, std::vector<std::pair<int, double> > &rhs)
{
   const size_t node_num_va = node_ptr_va->getNumber();
   const size_t node_num_vc = node_ptr_vc->getNumber();
   const size_t node_num_vx = node_ptr_vx->getNumber();

   const bool   is_gnd_node_va = node_ptr_va->isGROUND();
   const bool   is_gnd_node_vc = node_ptr_vc->isGROUND();
   const bool   is_gnd_node_vx = node_ptr_vx->isGROUND();

   const double va = (is_gnd_node_va) ? 0.0 : sol[node_num_va];
   const double vc = (is_gnd_node_vc) ? 0.0 : sol[node_num_vc];
   const double vx = (is_gnd_node_vx) ? 0.0 : sol[node_num_vx];

   const double unique4 = pow(R,(-1));
   const double ir = ((va - vx) * unique4);
   const double eva = ir;
   const double unique1 = (vx - vc);
   const double unique3 = (VT * n);
   const double unique2 = pow(unique3,(-1));
   const double unique0 = (unique1 * unique2);
   const double id = (-Is + (Is * cos(unique0)));
   const double evc = (-id);
   const double evx = (id - ir);

   if (!is_gnd_node_va)
      rhs.push_back(std::make_pair(node_num_va, eva));
   if (!is_gnd_node_vc)
      rhs.push_back(std::make_pair(node_num_vc, evc));
   if (!is_gnd_node_vx)
      rhs.push_back(std::make_pair(node_num_vx, evx));


   const double d_ir_d_va = unique4;
   const double eva_va = d_ir_d_va;
   const double d_ir_d_vx = (-unique4);
   const double eva_vx = d_ir_d_vx;
   const double unique8 = sin(unique0);
   const double unique7 = (Is * unique2 * unique8);
   const double d_id_d_vc = unique7;
   const double evc_vc = (-d_id_d_vc);
   const double d_id_d_vx = (-unique7);
   const double evc_vx = (-d_id_d_vx);
   const double evx_va = (-d_ir_d_va);
   const double evx_vc = d_id_d_vc;
   const double evx_vx = (-d_ir_d_vx + d_id_d_vx);

   if (!is_gnd_node_va)
   {
      mat.push_back(dsMath::RealRowColVal(node_num_va,node_num_va, eva_va));
      if (!is_gnd_node_vx)
         mat.push_back(dsMath::RealRowColVal(node_num_va,node_num_vx, eva_vx));
   }
   if (!is_gnd_node_vc)
   {
      mat.push_back(dsMath::RealRowColVal(node_num_vc,node_num_vc, evc_vc));
      if (!is_gnd_node_vx)
         mat.push_back(dsMath::RealRowColVal(node_num_vc,node_num_vx, evc_vx));
   }
   if (!is_gnd_node_vx)
   {
      if (!is_gnd_node_va)
         mat.push_back(dsMath::RealRowColVal(node_num_vx,node_num_va, evx_va));
      if (!is_gnd_node_vc)
         mat.push_back(dsMath::RealRowColVal(node_num_vx,node_num_vc, evx_vc));
      mat.push_back(dsMath::RealRowColVal(node_num_vx,node_num_vx, evx_vx));
   }

}


void mctest_user::assembleTran(const double scl, const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec *mat, std::vector<std::pair<int, double> > &rhs)
{
   const size_t node_num_vc = node_ptr_vc->getNumber();
   const size_t node_num_vx = node_ptr_vx->getNumber();

   const bool   is_gnd_node_vc = node_ptr_vc->isGROUND();
   const bool   is_gnd_node_vx = node_ptr_vx->isGROUND();

   const double vc = (is_gnd_node_vc) ? 0.0 : sol[node_num_vc];
   const double vx = (is_gnd_node_vx) ? 0.0 : sol[node_num_vx];

   const double unique1 = (vx - vc);
   const double qd = (C * unique1);
   const double evc = scl *(-qd);
   const double evx = scl *qd;

   if (!is_gnd_node_vc)
      rhs.push_back(std::make_pair(node_num_vc, evc));
   if (!is_gnd_node_vx)
      rhs.push_back(std::make_pair(node_num_vx, evx));


   if (mat == NULL)
      return;

   const double d_qd_d_vc = (-C);
   const double evc_vc = scl * (-d_qd_d_vc);
   const double d_qd_d_vx = C;
   const double evc_vx = scl * (-d_qd_d_vx);
   const double evx_vc = scl * d_qd_d_vc;
   const double evx_vx = scl * d_qd_d_vx;

   if (!is_gnd_node_vc)
   {
      mat->push_back(dsMath::RealRowColVal(node_num_vc,node_num_vc, evc_vc));
      if (!is_gnd_node_vx)
         mat->push_back(dsMath::RealRowColVal(node_num_vc,node_num_vx, evc_vx));
   }
   if (!is_gnd_node_vx)
   {
      if (!is_gnd_node_vc)
         mat->push_back(dsMath::RealRowColVal(node_num_vx,node_num_vc, evx_vc));
      mat->push_back(dsMath::RealRowColVal(node_num_vx,node_num_vx, evx_vx));
   }

}

bool mctest_user::addParam(const std::string &str, double val)
{
    bool ret = false;
    if (str == "C")
    {
        C = val;
        ret = true;
    }
    else if (str == "Is")
    {
        Is = val;
        ret = true;
    }
    else if (str == "R")
    {
        R = val;
        ret = true;
    }
    else if (str == "VT")
    {
        VT = val;
        ret = true;
    }
    else if (str == "n")
    {
        n = val;
        ret = true;
    }
    return ret;
}

extern "C" InstanceModel *mctest_user_create (NodeKeeper *nk, const std::string &name, const std::vector<std::string> &nodelist) {
 return new mctest_user(nk, name.c_str(), nodelist[0].c_str(), nodelist[1].c_str());

}
