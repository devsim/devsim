from auditwheel.main import main
import auditwheel.repair
import sys

if hasattr(auditwheel.repair, '_is_valid_rpath') and callable(getattr(auditwheel.repair, '_is_valid_rpath')):
    auditwheel.repair._is_valid_rpath = lambda x, y, z : True
else:
    raise RuntimeError("Something is wrong with auditwheel customization")

if __name__ == '__main__':
    sys.exit(main())
