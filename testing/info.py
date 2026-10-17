# Copyright 2020 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

####
#### info.py
#### print features for this build
####
import devsim

for k, v in devsim.get_parameter(name="info").items():
    if isinstance(v, str):
        print(f'"{k}"\t"{v}"')
    else:
        print(f'"{k}"\t{v}')
