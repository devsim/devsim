
import umfpack_loader as umf

def init(**kwargs):
    '''
    initializations and returns class
    '''
    gdata = umf.global_data()
    gdata.dll = umf.load_umfpack_dll()
    blas_dll = kwargs["blas"]
    if isinstance(blas_dll, str):
        blas_dll = (blas_dll,)
    mcount = -1
    for b in blas_dll:
        h = umf.load_blas_dll(gdata, blaslib=b, noexcept=True)
        if h:
            mcount = umf.load_blas_functions(gdata, h)
            if mcount == 0:
                break
    if mcount != 0:
        raise RuntimeError('Missing %d math functions' % mcount)
    uc = umf.umf_control(gdata, kwargs['matrix_type'])
    return uc

def matrix(**kwargs):
    if kwargs["matrix_format"] != "csc":
        raise RuntimeError("Expected csc format instead of %s" % kw_args["matrix_format"])
    n = len(kwargs["Ap"]) - 1
    return umf.di_matrix(uc=kwargs["math_object"], Ap=kwargs["Ap"], Ai=kwargs["Ai"], Ax=kwargs["Ax"])

def symbolic(**kwargs):
    return kwargs["math_object"].symbolic(matrix=kwargs["matrix"])

def numeric(**kwargs):
    return kwargs["math_object"].numeric(matrix=kwargs["matrix"], Symbolic=kwargs["symbolic"])

def numeric(**kwargs):
    return kwargs["math_object"].numeric(matrix=kwargs["matrix"], Symbolic=kwargs["symbolic"])

def solve(**kwargs):
    return kwargs["math_object"].solve(matrix=kwargs["matrix"], Numeric=kwargs["numeric"], x=kwargs["x"], b=kwargs["b"], transpose=kwargs["transpose"])

def status(**kwargs):
    return kwargs["math_object"].status

action_table = {
    'init' : init,
    'matrix' : matrix,
    'symbolic' : symbolic,
    'numeric' : numeric,
    'status' : status,
    'solve' : solve,
}


def solver(**kwargs):
    return action_table[kwargs['action']](**kwargs)

