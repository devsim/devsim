# Copyright 2013 Devsim LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

proc callSymdiff {arg} {
puts [format "\"%s\"\n\"%s\"\n\"%s\"\n\n" $arg [symdiff -expr "$arg;"] [symdiff -expr "diff($arg,x);"]]
}

callSymdiff "min(x*b, x+b)"
callSymdiff "min(x*b, x+!b)"
callSymdiff "max(x*b, x+b)"
callSymdiff "asinh(x)"
callSymdiff "acosh(x)"
callSymdiff "atanh(x)"

callSymdiff "exp(0)"
callSymdiff "simplify(exp(0))"
callSymdiff "exp(1)"
callSymdiff "exp(2)"

callSymdiff "pow(x,0)"
callSymdiff "simplify(pow(x,0))"
callSymdiff "pow(x,1)"
callSymdiff "simplify(pow(x,1))"
callSymdiff "pow(x*y,1)"
callSymdiff "pow(x*y,2)"
callSymdiff "pow(x,2)"
callSymdiff "pow(2,x)"
callSymdiff "pow(2,x*y)"
callSymdiff "pow(2,3)"
callSymdiff "pow(0,x)"
callSymdiff "simplify(pow(0,x))"
callSymdiff "pow(1,x)"
callSymdiff "simplify(pow(1,x))"

callSymdiff "exp(x)"
callSymdiff "exp(x*y)"
callSymdiff "log(x)"
callSymdiff "log(x*y)"
callSymdiff "log(1)"
callSymdiff "x*y"
callSymdiff "x"
callSymdiff "0*x"
callSymdiff "0*x*y"
callSymdiff "x*y*z"
callSymdiff "0+x+y"
callSymdiff "pow(x*y,z)"
callSymdiff "pow(y,x*z)"
callSymdiff "pow(x*y,x*z)"
