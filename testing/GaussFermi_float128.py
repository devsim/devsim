# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

#Purpose: Fermi integral and inverse

import devsim
import test_common
import sys

if not devsim.get_parameter(name='info')['extended_precision']:
    print("Extended precision support is not available with this version")
    sys.exit(0)

devsim.set_parameter(name = "extended_solver", value=True)
devsim.set_parameter(name = "extended_model", value=True)
devsim.set_parameter(name = "extended_equation", value=True)

import GaussFermi

