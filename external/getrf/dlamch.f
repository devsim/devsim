*> \brief \b quad_dlamch
*
*  =========== DOCUMENTATION ===========
*
* Online html documentation available at
*            http://www.netlib.org/lapack/explore-html/
*
*  Definition:
*  ===========
*
*      DOUBLE PRECISION FUNCTION quad_dlamch( CMACH )
*
*
*> \par Purpose:
*  =============
*>
*> \verbatim
*>
*> quad_dlamch determines double precision machine parameters.
*> \endverbatim
*
*  Arguments:
*  ==========
*
*> \param[in] CMACH
*> \verbatim
*>          Specifies the value to be returned by quad_dlamch:
*>          = 'E' or 'e',   quad_dlamch := eps
*>          = 'S' or 's ,   quad_dlamch := sfmin
*>          = 'B' or 'b',   quad_dlamch := base
*>          = 'P' or 'p',   quad_dlamch := eps*base
*>          = 'N' or 'n',   quad_dlamch := t
*>          = 'R' or 'r',   quad_dlamch := rnd
*>          = 'M' or 'm',   quad_dlamch := emin
*>          = 'U' or 'u',   quad_dlamch := rmin
*>          = 'L' or 'l',   quad_dlamch := emax
*>          = 'O' or 'o',   quad_dlamch := rmax
*>          where
*>          eps   = relative machine precision
*>          sfmin = safe minimum, such that 1/sfmin does not overflow
*>          base  = base of the machine
*>          prec  = eps*base
*>          t     = number of (base) digits in the mantissa
*>          rnd   = 1.0 when rounding occurs in addition, 0.0 otherwise
*>          emin  = minimum exponent before (gradual) underflow
*>          rmin  = underflow threshold - base**(emin-1)
*>          emax  = largest exponent before overflow
*>          rmax  = overflow threshold  - (base**emax)*(1-eps)
*> \endverbatim
*
*  Authors:
*  ========
*
*> \author Univ. of Tennessee
*> \author Univ. of California Berkeley
*> \author Univ. of Colorado Denver
*> \author NAG Ltd.
*
*> \date December 2016
*
*> \ingroup auxOTHERauxiliary
*
*  =====================================================================
      DOUBLE PRECISION FUNCTION quad_dlamch( CMACH )
*
*  -- LAPACK auxiliary routine (version 3.7.0) --
*  -- LAPACK is a software package provided by Univ. of Tennessee,    --
*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..--
*     December 2016
*
*     .. Scalar Arguments ..
      CHARACTER          CMACH
*     ..
*
* =====================================================================
*
*     .. Parameters ..
      DOUBLE PRECISION   ONE, ZERO
      PARAMETER          ( ONE = 1.0D+0, ZERO = 0.0D+0 )
*     ..
*     .. Local Scalars ..
      DOUBLE PRECISION   RND, EPS, SFMIN, SMALL, RMACH
*     ..
*     .. External Functions ..
      LOGICAL            quad_lsame
      EXTERNAL           quad_lsame
*     ..
*     .. Intrinsic Functions ..
      INTRINSIC          DIGITS, EPSILON, HUGE, MAXEXPONENT,
     $                   MINEXPONENT, RADIX, TINY
*     ..
*     .. Executable Statements ..
*
*
*     Assume rounding, not chopping. Always.
*
      RND = ONE
*
      IF( ONE.EQ.RND ) THEN
         EPS = EPSILON(ZERO) * 0.5
      ELSE
         EPS = EPSILON(ZERO)
      END IF
*
      IF( quad_lsame( CMACH, 'E' ) ) THEN
         RMACH = EPS
      ELSE IF( quad_lsame( CMACH, 'S' ) ) THEN
         SFMIN = TINY(ZERO)
         SMALL = ONE / HUGE(ZERO)
         IF( SMALL.GE.SFMIN ) THEN
*
*           Use SMALL plus a bit, to avoid the possibility of rounding
*           causing overflow when computing  1/sfmin.
*
            SFMIN = SMALL*( ONE+EPS )
         END IF
         RMACH = SFMIN
      ELSE IF( quad_lsame( CMACH, 'B' ) ) THEN
         RMACH = RADIX(ZERO)
      ELSE IF( quad_lsame( CMACH, 'P' ) ) THEN
         RMACH = EPS * RADIX(ZERO)
      ELSE IF( quad_lsame( CMACH, 'N' ) ) THEN
         RMACH = DIGITS(ZERO)
      ELSE IF( quad_lsame( CMACH, 'R' ) ) THEN
         RMACH = RND
      ELSE IF( quad_lsame( CMACH, 'M' ) ) THEN
         RMACH = MINEXPONENT(ZERO)
      ELSE IF( quad_lsame( CMACH, 'U' ) ) THEN
         RMACH = tiny(zero)
      ELSE IF( quad_lsame( CMACH, 'L' ) ) THEN
         RMACH = MAXEXPONENT(ZERO)
      ELSE IF( quad_lsame( CMACH, 'O' ) ) THEN
         RMACH = HUGE(ZERO)
      ELSE
         RMACH = ZERO
      END IF
*
      quad_dlamch = RMACH
      RETURN
*
*     End of quad_dlamch
*
      END
************************************************************************
*> \brief \b quad_dlamc3
*> \details
*> \b Purpose:
*> \verbatim
*> quad_dlamc3  is intended to force  A  and  B  to be stored prior to doing
*> the addition of  A  and  B ,  for use in situations where optimizers
*> might hold one of these in a register.
*> \endverbatim
*> \author LAPACK is a software package provided by Univ. of Tennessee, Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..
*> \date December 2016
*> \ingroup auxOTHERauxiliary
*>
*> \param[in] A
*> \verbatim
*>          A is a DOUBLE PRECISION
*> \endverbatim
*>
*> \param[in] B
*> \verbatim
*>          B is a DOUBLE PRECISION
*>          The values A and B.
*> \endverbatim
*>
      DOUBLE PRECISION FUNCTION quad_dlamc3( A, B )
*
*  -- LAPACK auxiliary routine (version 3.7.0) --
*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd..
*     November 2010
*
*     .. Scalar Arguments ..
      DOUBLE PRECISION   A, B
*     ..
* =====================================================================
*
*     .. Executable Statements ..
*
      quad_dlamc3 = A + B
*
      RETURN
*
*     End of quad_dlamc3
*
      END
*
************************************************************************
