MODULE LA_DFT
    USE, INTRINSIC :: ISO_C_BINDING
    IMPLICIT NONE

    REAL(C_DOUBLE), PARAMETER :: PI = ACOS(-1.0)
    CONTAINS

    INTEGER(C_INT) FUNCTION LA_UNI_DFT_NP(cx, nx, cy, ny, cw) &
        RESULT(CODE) &
        BIND(C, NAME="LAUNILATERAL_DFT_NO_PHASE")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cy
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), VALUE, INTENT(IN)    :: nx
        INTEGER(C_SIZE_T), VALUE, INTENT(IN)    :: ny
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: y(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        INTEGER(8)                              :: i
        REAL(C_DOUBLE)                          :: nx_r
        REAL(C_DOUBLE)                          :: re, im, omega, a
        REAL(C_DOUBLE)                          :: c_j, s_j, c, s, tmp
        INTEGER(8)                              :: j

        CALL C_F_POINTER(cx, x, SHAPE=[nx])
        CALL C_F_POINTER(cy, y, SHAPE=[ny])
        CALL C_F_POINTER(cw, w, SHAPE=[nx])

        nx_r = nx

        !$omp parallel do default(none) &
        !$omp shared(x, y, w, nx, ny, nx_r) &
        !$omp private(i, j, re, im, omega, a, c_j, s_j, c, s, tmp)
        DO i=1, ny
            re = 0.0_C_DOUBLE
            im = 0.0_C_DOUBLE
            omega = 2.0_C_DOUBLE * PI * (i-1) / nx_r 
            c_j = COS(omega)
            s_j = SIN(omega)
            c  = 1.0_C_DOUBLE
            s  = 0.0_C_DOUBLE

            DO j=1, nx
                a  = x(j) * w(j)
                !re = re + (a * COS((j - 1) * omega))
                !im = im - (a * SIN((j - 1) * omega))
                re = re + a * c
                im = im - a * s

                !rotation
                tmp = c*c_j - s*s_j
                s   = s*c_j + c*s_j
                c   = tmp
            END DO

            y(i) = SQRT(re**2 + im**2)
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LA_UNI_DFT_NP

    INTEGER(C_INT) FUNCTION LA_BI_DFT_NP(cx, nx, cy, ny, cw) &
        RESULT(CODE) &
        BIND(C, NAME="LABILATERAL_DFT_NO_PHASE")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cy
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), VALUE, INTENT(IN)    :: nx
        INTEGER(C_SIZE_T), VALUE, INTENT(IN)    :: ny
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: y(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        INTEGER(8)                              :: i
        REAL(C_DOUBLE)                          :: nx_r
        REAL(C_DOUBLE)                          :: re, im, omega, a
        INTEGER(8)                              :: j

        CALL C_F_POINTER(cx, x, SHAPE=[nx])
        CALL C_F_POINTER(cy, y, SHAPE=[ny])
        CALL C_F_POINTER(cw, w, SHAPE=[nx])

        nx_r = nx

        !$omp parallel do default(none) &
        !$omp shared(x, y, w, nx, ny, nx_r) &
        !$omp private(i, j, re, im, omega, a)
        DO i=1, ny
            re = 0.0_C_DOUBLE
            im = 0.0_C_DOUBLE
            omega = 2.0_C_DOUBLE * PI * (-(nx/2) + (i-1)) / nx_r 

            DO j=1, MIN(nx, nx)
                a  = x(j) * w(j)
                re = re + (a * COS((j - 1) * omega))
                im = im - (a * SIN((j - 1) * omega))
            END DO

            y(i) = SQRT(re**2 + im**2)
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LA_BI_DFT_NP

    INTEGER(C_INT) FUNCTION LA_BI_DFT(cx, nx, cy, cp, ny, cw) &
        RESULT(CODE) &
        BIND(C, NAME="LABILATERAL_DFT")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cy
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cp
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), VALUE, INTENT(IN)    :: nx
        INTEGER(C_SIZE_T), VALUE, INTENT(IN)    :: ny
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: y(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: p(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        INTEGER(8)                              :: i
        REAL(C_DOUBLE)                          :: nx_r
        REAL(C_DOUBLE)                          :: re, im, omega, a
        INTEGER(8)                              :: j

        CALL C_F_POINTER(cx, x, SHAPE=[nx])
        CALL C_F_POINTER(cy, y, SHAPE=[ny])
        CALL C_F_POINTER(cp, y, SHAPE=[ny])
        CALL C_F_POINTER(cw, w, SHAPE=[nx])

        nx_r = nx

        !$omp parallel do default(none) &
        !$omp shared(x, y, p, w, nx, ny, nx_r) &
        !$omp private(i, j, re, im, omega, a)
        DO i=1, ny
            re = 0.0_C_DOUBLE
            im = 0.0_C_DOUBLE
            omega = 2.0_C_DOUBLE * PI * (-(nx/2) + (i-1)) / nx_r 

            DO j=1, MIN(nx, nx)
                a  = x(j) * w(j)
                re = re + (a * COS((j - 1) * omega))
                im = im - (a * SIN((j - 1) * omega))
            END DO

            y(i) = SQRT(re**2 + im**2)
            p(i) = ATAN2(im, re)
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LA_BI_DFT

END MODULE LA_DFT
