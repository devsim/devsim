import devsim
from . import umfpack_loader as umf
import sys
import os
import array

class dsobject:
    def __init__(self, n, transpose):
        self.n = n
        self.transpose = transpose
        self.status = True
        self.message = None
        # this may not be needed to save
        self.matrix = None
        self.gdata = None
        self.initialize_umfpack()
        self.umf_control = None
        self.x = None

    def __del__(self):
        self.matrix=None
        self.umf_control = None
        self.gdata=None

    def initialize_umfpack(self):
        self.gdata = umf.global_data()
        self.gdata.dll = umf.load_umfpack_dll(os.path.join(os.path.dirname(__file__), umf.get_umfpack_name()))
        for b in devsim.get_parameter(name='info')['math_libraries']:
            h = umf.load_blas_dll(self.gdata, blaslib=b, noexcept=True)
            if h:
                mcount = umf.load_blas_functions(self.gdata, h)
                if mcount == 0:
                    break
        if mcount != 0:
            raise RuntimeError('Missing %d math functions' % mcount)

    def factor(self, **kwargs):
        self.status = False
        self.message = ''
        is_complex = kwargs['is_complex']
        if not self.umf_control:
            if is_complex:
                self.umf_control = umf.umf_control(self.gdata, 'complex')
            else:
                self.umf_control = umf.umf_control(self.gdata, 'real')
            #self.umf_control.init_verbose()
        # test same symbolic
        self.matrix = umf.matrix(uc=self.umf_control, Ap=kwargs['Ap'], Ai=kwargs["Ai"], Ax=kwargs["Ax"])
        if not kwargs['is_same_symbolic']:
          self.symbolic = self.umf_control.symbolic(matrix=self.matrix)
        self.numeric = self.umf_control.numeric(matrix=self.matrix, Symbolic=self.symbolic)
        self.status = True
        self.message = ''

    def solve(self, **kwargs):
        self.status = False
        self.message = ''
        if self.umf_control.is_complex:
            #print(kwargs.keys())
            #raise RuntimeError("COMPLEX!")
            self.x = array.array('d', [0.0])*len(kwargs['b'])
            self.umf_control.solve(matrix=self.matrix, Numeric=self.numeric, b=kwargs['b'], transpose=self.transpose, x=self.x)
        else:
            self.x = array.array('d', kwargs['b'])
            self.umf_control.solve(matrix=self.matrix, Numeric=self.numeric, b=kwargs['b'], transpose=self.transpose, x=self.x)
        self.status = True

def local_solver_callback(**kwargs):
    #print(kwargs['action'])
    if (kwargs['action'] == 'init'):
        so =  dsobject(
              n=kwargs['n'],
              transpose=kwargs['transpose']
          )
        return {
          'solver_object' : so,
          'matrix_format' : "csc",
          'status' : so.status,
          'message' : so.message,
        }
    elif kwargs['action'] == 'factor':
        so = kwargs['solver_object']
        so.factor(**kwargs)
        return {
          'status' : so.status,
          'message' : so.message,
        }
    elif kwargs['action'] == 'solve':
        so = kwargs['solver_object']
        so.solve(**kwargs)
        if kwargs['is_complex']:
            return {
              'status' : so.status,
              'message' : so.message,
              'x' : so.x,
            }
        else:
            return {
              'status' : so.status,
              'message' : so.message,
              'x' : so.x,
            }
    else:
        raise RuntimeError('Unsupported action, ' + kwargs['action'])
    return False


devsim.set_parameter(name="direct_solver", value="custom")
devsim.set_parameter(name="solver_callback", value=local_solver_callback)
#print(devsim.get_parameter(name="info"))

if __name__ == "__main__":
  #print(sys.argv)
  sys.argv = sys.argv[1:]
  sys.path.append(os.path.dirname(sys.argv[0]))
  exec(compile(open(sys.argv[0], "rb").read(), sys.argv[0], 'exec'))
