/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Signal.hh"
#include <cfloat>
#include <vector>
#include <algorithm>
#include <cmath>

Signal::Signal(SignalType x) : _type(x) {}

DCOnly::DCOnly(double x) : Signal(SignalType::DCONLY) {
   _volt=x;
}

double DCOnly::NextBreakPoint(double)
{
   return DBL_MAX;
}

double DCOnly::CalcVoltage(double)
{
   return _volt;
}

Pulse::Pulse(double v1,
      double v2,
      double td,
      double tr,
      double tf,
      double pw,
      double per) : Signal(SignalType::PULSE), // add sanity checks later
         _v1(v1),
         _v2(v2),
         _td(td),
         _tr(tr),
         _tf(tf),
         _pw(pw),
         _per(per)
{
         _volt=v1;
}

// Need to make best numerical accuracy possible
// return time of the next break point, not time until
double Pulse::NextBreakPoint(double tnow)
{
   // number of periods already traversed
   // not considering td
   double numper = floor((tnow-_td) / _per);

   double adj = (numper - 1.0) * _per;

   // This eliminates all of the redundant cycles
   double t = tnow - adj;

   // don't need to correct for number of periods
   if (t < _td)
      return _td;

   double ans = 0.0;
   // Add smallest numbers first
   std::vector<double> tpts;
   tpts.push_back(adj);
   tpts.push_back(_td);
   tpts.push_back(_tr);
   std::sort(tpts.begin(),tpts.end());
   for (size_t i = tpts.size(); i != 0; --i)
      ans += tpts[i-1];

   if (tnow < ans)
      return ans;

   ans=0.0;
   tpts.push_back(_pw);
   std::sort(tpts.begin(),tpts.end());
   for (size_t i = tpts.size(); i != 0; --i)
      ans += tpts[i-1];

   if (tnow < ans)
      return ans;

   ans = 0.0;
   tpts.push_back(_tf);
   std::sort(tpts.begin(),tpts.end());
   for (size_t i = tpts.size(); i != 0; --i)
      ans += tpts[i-1];

   if (tnow < ans)
      return ans;

   ans = 0.0;
   tpts.clear();
   tpts.push_back(_td);
   tpts.push_back(adj);
   tpts.push_back(_per);
   std::sort(tpts.begin(),tpts.end());
   for (size_t i = tpts.size(); i != 0; --i)
      ans += tpts[i-1];

   if (t < ans)
      return ans;

   ans = 0.0;
   return ans;

}

double Pulse::CalcVoltage(double tnow)
{
   // don't need to correct for number of periods
   if (tnow < _td)
      return _volt=_v1;

   // number of periods already traversed
   // not considering td
   double numper = floor((tnow-_td) / _per);

   double adj = (numper) * _per;


   double ans = 0.0;
   // Add smallest numbers first
   std::vector<double> tpts;
   tpts.push_back(adj);
   tpts.push_back(_td);
   tpts.push_back(_tr);
   std::sort(tpts.begin(),tpts.end());
   for (size_t i = tpts.size(); i != 0; --i)
      ans += tpts[i-1];

   if (tnow < ans)
      return _volt = (_v2-_v1)/(_tr)*(tnow-ans)+_v2;

   ans=0.0;
   tpts.push_back(_pw);
   std::sort(tpts.begin(),tpts.end());
   for (size_t i = tpts.size(); i != 0; --i)
      ans += tpts[i-1];

   if (tnow < ans)
      return _volt = _v2;

   ans = 0.0;
   tpts.push_back(_tf);
   std::sort(tpts.begin(),tpts.end());
   for (size_t i = tpts.size(); i != 0; --i)
      ans += tpts[i-1];

   if (tnow < ans)
      return _volt = _v1 + (_v1-_v2)/(_tf)*(tnow-ans);

   return _volt = _v1;

}
