MODULE CLAMP_FUNCS
    USE, INTRINSIC :: ISO_C_BINDING
    IMPLICIT NONE

    REAL(C_DOUBLE), PARAMETER :: PI = ACOS(-1.0)
    CONTAINS

    REAL(C_DOUBLE) FUNCTION MIDPOINT(a, b) RESULT (mid)
        REAL(C_DOUBLE), INTENT(IN)          :: a
        REAL(C_DOUBLE), INTENT(IN)          :: b
        mid = 0.5 * (b + a)
        RETURN
    END FUNCTION MIDPOINT

    REAL(C_DOUBLE) FUNCTION MIDSIZE(a, b) RESULT (mid)
        REAL(C_DOUBLE), INTENT(IN)          :: a
        REAL(C_DOUBLE), INTENT(IN)          :: b
        mid = 0.5 * (b - a)
        RETURN
    END FUNCTION MIDSIZE

    INTEGER(C_INT) FUNCTION LACLAMP_NO_CLAMP(cx, xn, cy, yn) RESULT(code) &
        BIND(C, NAME="LACLAMP_NO_CLAMP")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)       :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)       :: cy
        INTEGER(C_SIZE_T), INTENT(IN)        :: xn
        INTEGER(C_SIZE_T), INTENT(IN)        :: yn
        INTEGER(8)                           :: i
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        code = 0
        DO CONCURRENT (i=1:MIN(xn, yn))
            y(i) = x(i)
        END DO

        RETURN
    END FUNCTION LACLAMP_NO_CLAMP

    INTEGER(C_INT) FUNCTION LACLAMP_HARD_CLIP(cx, xn, cy, yn, a, b) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_HARD_CLIP")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        DO CONCURRENT (i=1:MIN(xn, yn))
            y(i) = MAX(MIN(x(i), b), a)
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_HARD_CLIP

    INTEGER(C_INT) FUNCTION LACLAMP_SOFT_CLIP(cx, xn, cy, yn, a, b, k)&
        RESULT(code) &
        BIND(C, NAME="LACLAMP_SOFT_CLIP")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            y(i) = q * TANH(k * (x(i) - p) / q) + p
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_SOFT_CLIP

    INTEGER(C_INT) FUNCTION LACLAMP_HARD_FOLD(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_HARD_FOLD")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        !REAL(C_DOUBLE)                     :: t
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        !This might not work concurrently
        DO CONCURRENT (i=1:MIN(xn, yn))
            !t = k * (x(i) - p) / (4 * q)
            !t = t - FLOOR(t)
            !y(i) = q * MAX(MIN(4*t, 2 - 4*t), -4 + 4*t) + p
            y(i) = q * (2/PI) * ASIN(SIN(0.5 * PI * k * (x(i)-p) / q)) + p
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_HARD_FOLD

    INTEGER(C_INT) FUNCTION LACLAMP_SOFT_FOLD(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_SOFT_FOLD")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            y(i) = q * SIN(PI * 0.5 * k * (x(i) - p) / q) + p
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_SOFT_FOLD

    INTEGER(C_INT) FUNCTION LACLAMP_MODULO(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_MODULO")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            y(i) = 2*q * MOD(k *(x(i) - a) / (b - a), 1.0_C_DOUBLE) + a
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_MODULO

    INTEGER(C_INT) FUNCTION LACLAMP_SOFT_SIGN(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_SOFT_SIGN")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            y(i) = p + q * k * (x(i) - p) / (1 + k * ABS(x(i) - p))
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_SOFT_SIGN

    INTEGER(C_INT) FUNCTION LACLAMP_CUBIC_CLAMP(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_CUBIC_CLAMP")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t
                t    = (x(i) - p) / q
                y(i) = MAX(MIN((t*t*t + (k*t)) / (1 + k), b), a)
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_CUBIC_CLAMP

    INTEGER(C_INT) FUNCTION LACLAMP_SIMPLE_FOLDOVER(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_SIMPLE_FOLDOVER")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            y(i) = MIN(MAX(-k*x(i) + 2*a, k * x(i)), -k*x(i) + 2*b)
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_SIMPLE_FOLDOVER

    INTEGER(C_INT) FUNCTION LACLAMP_DOUBLE_CUSP_FOLD(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_DOUBLE_CUSP_FOLD")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t
                REAL(C_DOUBLE) :: t2
                t  = k * (x(i) - p) / q
                t2 = -(ABS(t) - 1)**2 + 1
                y(i) = q * SIGN(1.0_C_DOUBLE, t) * t2 +p
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_DOUBLE_CUSP_FOLD

    INTEGER(C_INT) FUNCTION LACLAMP_EXPONENTIAL_FOLD(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_EXPONENTIAL_FOLD")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t
                t = (x(i) + 1) / 4
                t = t - FLOOR(t)
                t = MERGE(-2 * (t - 0.5), 2*t, t >= 0.5)
                t = 0.5*(SIGN(1.0_C_DOUBLE, -t) + 1) + SIGN(1.0_C_DOUBLE,t) * &
                    (1 - EXP(-k*ABS(t))) / (1 - EXP(-k))
                y(i) = q * (2*t - 1) + p 
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_EXPONENTIAL_FOLD

    INTEGER(C_INT) FUNCTION LACLAMP_PARTIAL_WRAP(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_PARTIAL_WRAP")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
            REAL (C_DOUBLE) :: t
            REAL (C_DOUBLE) :: t2
            t = k *(x(i) - p) / q
            t2 = t - FLOOR(t)
            t2 = MERGE(t2, t2-1, t > 0)
            y(i) = q * t2 + p
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_PARTIAL_WRAP

    INTEGER(C_INT) FUNCTION &
        LACLAMP_SYMMETRICAL_LOGISTIC(cx, xn, cy, yn, a, b, k) RESULT(code) &
        BIND(C, NAME="LACLAMP_SYMMETRICAL_LOGISTIC")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
            REAL(C_DOUBLE) :: t
            t = (x(i) - p) / q
            y(i) = -q + p + (2*q) / (1 + EXP(-k * t))
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_SYMMETRICAL_LOGISTIC

    INTEGER(C_INT) FUNCTION LACLAMP_S_CURVE(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_S_CURVE")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        REAL(C_DOUBLE)                     :: k_3
        REAL(C_DOUBLE)                     :: e_3
        REAL(C_DOUBLE)                     :: norm
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)
        e_3 = 1.0_C_DOUBLE / 3.0_C_DOUBLE
        k_3 = k**(e_3)
        norm = (1+k)**(e_3) - k_3

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t
                REAL(C_DOUBLE) :: u
                t = (x(i) - p) / q
                u = EXP(e_3 * LOG(ABS(t) + k)) - k_3
                y(i) = q * SIGN(1.0_C_DOUBLE, t) * (u / norm) +  p
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_S_CURVE

    INTEGER(C_INT) FUNCTION &
        LACLAMP_EXPONENTIAL_GATE(cx, xn, cy, yn, a, b, k) RESULT(code) &
        BIND(C, NAME="LACLAMP_EXPONENTIAL_GATE")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t
                t = (x(i) - p) / q
                t = t * (1 + EXP(-k * (q - ABS(t)))) * 0.5
                y(i) = q * t + p
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_EXPONENTIAL_GATE

    INTEGER(C_INT) FUNCTION LACLAMP_SOFT_KNEE(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_SOFT_KNEE")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t1
                REAL(C_DOUBLE) :: t2
                t1 = b + (LOG(1 + k*(x(i) - b)) / k)
                t2 = a - (LOG(1 + k*(-(x(i) - a))) / k)
                y(i) = MERGE(MERGE(t1, x(i), x(i) > b), t2, x(i) > a)
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_SOFT_KNEE

    INTEGER(C_INT) FUNCTION &
        LACLAMP_SATURATED_LOGISTIC_MAP(cx, xn, cy, yn, a, b, k) RESULT(code) &
        BIND(C, NAME="LACLAMP_SATURATED_LOGISTIC_MAP")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            ! HERE WILL BE DRAGONS
            y(i) = q * SIN(PI * 0.5 * k * (x(i) - p) / q) + p
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_SATURATED_LOGISTIC_MAP

    INTEGER(C_INT) FUNCTION LACLAMP_TENT_MAP(cx, xn, cy, yn, a, b, mu, y0) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_TENT_MAP")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: mu
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: y0
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        REAL(C_DOUBLE)                     :: t
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO i=1, MIN(xn, yn)
            IF (i > 1) THEN
                t = (0.5*(x(i) + y(i-1)) - p) / q
                t = mu * MIN(t, 1 - t)
                y(i) = q * t + p
            ELSE
                y(i) = y0
            END IF
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_TENT_MAP

    INTEGER(C_INT) FUNCTION LACLAMP_BJT_SATURATION(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_BJT_SATURATION")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            y(i) = p + q * (1 - EXP(-k * (x(i) - p)))
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_BJT_SATURATION

    INTEGER(C_INT) FUNCTION &
        LACLAMP_SLEW_RATE_SATURATION(cx, xn, cy, yn, v, y0) RESULT(code)&
        BIND(C, NAME="LACLAMP_SLEW_RATE_SATURATION")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: v
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: y0
        REAL(C_DOUBLE)                     :: dx
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        DO i=1, MIN(xn, yn)
            IF (i > 1) THEN
                dx = x(i) - y(i-1)
                dx = MIN(MAX(dx, -v), v)
                y(i) = y(i-1) + dx
            ELSE
                y(i) = y0
            END IF
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_SLEW_RATE_SATURATION

    INTEGER(C_INT) FUNCTION &
        LACLAMP_SCHMITT_TRIGGER(cx, xn, cy, yn, a, b, t, y0) RESULT(code) &
        BIND(C, NAME="LACLAMP_SCHMITT_TRIGGER")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: t
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: y0
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        REAL(C_DOUBLE)                     :: t2
        REAL(C_DOUBLE)                     :: x1
        REAL(C_DOUBLE)                     :: y1
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)
        t2 = t * 0.5

        DO i=1, MIN(xn, yn)
            IF (i > 1) THEN
                x1 = (x(i) - p) / q
                y1 = y(i-1)

                IF (y1 <= p .AND. x1 >= (1-t2)) THEN
                    y(i) = b
                ELSE IF (y1 > p .AND. x1 <= t2) THEN
                    y(i) = a
                ELSE
                    y(i) = y1
                END IF
            ELSE
                y(i) = y0
            END IF
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_SCHMITT_TRIGGER

    INTEGER(C_INT) FUNCTION LACLAMP_RC_LOW_PASS(cx, xn, cy, yn, a, b, r, c, v0)&
        RESULT(code) &
        BIND(C, NAME="LACLAMP_RC_LOW_PASS")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: r
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: c
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: v0
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            y(i) = q * SIN(PI * 0.5 * r *v0 * c * (x(i) - p) / q) + p
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_RC_LOW_PASS

    INTEGER(C_INT) FUNCTION &
        LACLAMP_SYMMETRICAL_DIODE(cx, xn, cy, yn, a, b, i_s, k) RESULT(code) &
        BIND(C, NAME="LACLAMP_SYMMETRICAL_DIODE")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: i_s
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t
                REAL(C_DOUBLE) :: u
                t = (x(i) - p) / q
                u = k * LOG(1 + (ABS(t) / i_s))
                y(i) = SIGN(1.0_C_DOUBLE, t) * q * u + p
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_SYMMETRICAL_DIODE

    INTEGER(C_INT) FUNCTION &
        LACLAMP_UNILATERAL_DIODE(cx, xn, cy, yn, a, b, i_s, k) RESULT(code) &
        BIND(C, NAME="LACLAMP_UNILATERAL_DIODE")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: i_s
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t
                t = (x(i) - p) / q
                t = k * LOG(1 + (t / i_s))
                y(i) = MERGE(p, q * t + p, x(i) < p)
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_UNILATERAL_DIODE

    INTEGER(C_INT) FUNCTION &
        LACLAMP_DIFFERENTIATOR(cx, xn, cy, yn, y0, h) RESULT(code) &
        BIND(C, NAME="LACLAMP_DIFFERENTIATOR")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: y0
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: h
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        DO i=1, MIN(xn, yn)
            IF (i > 1) THEN
                y(i) = (x(i) - x(i-1)) / h
            ELSE
                y(i) = y0
            END IF
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_DIFFERENTIATOR

    INTEGER(C_INT) FUNCTION &
        LACLAMP_INTEGRATOR(cx, xn, cy, yn, y0, h) RESULT(code) &
        BIND(C, NAME="LACLAMP_INTEGRATOR")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: y0
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: h
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        DO i=1, MIN(xn, yn)
            IF (i > 1) THEN
                y(i) = y(i-1) + (x(i) + x(i-1)) * 0.5 * h
            ELSE
                y(i) = y0
            END IF
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_INTEGRATOR

    INTEGER(C_INT) FUNCTION LACLAMP_STOCHASTIC(cx, xn, cy, yn, a, b) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_STOCHASTIC")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)      :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)      :: cy
        INTEGER(C_SIZE_T), INTENT(IN)       :: xn
        INTEGER(C_SIZE_T), INTENT(IN)       :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)   :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)   :: b
        INTEGER(8)                          :: n
        INTEGER(8)                          :: z1
        INTEGER(8)                          :: z2
        INTEGER(8)                          :: z3
        REAL(C_DOUBLE)                      :: r3
        INTEGER(8)                          :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        n  = -559038737 ! 0xDEADBEEF
        z1 = 1103515245
        z2 = 12345
        z3 = 2**30
        r3 = z3

        DO i=1, MIN(xn, yn)
            n = MOD(z1 * n + z2, z3)
            y(i) = MIN(MAX(-(n / r3) * x(i) + (b + a)*0.5, a), b)
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_STOCHASTIC

    INTEGER(C_INT) FUNCTION &
        LACLAMP_EVEN_POWER_LIMITER(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_EVEN_POWER_LIMITER")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i
        INTEGER(8)                         :: j
        REAL(C_DOUBLE) :: t_a
        REAL(C_DOUBLE) :: t_b
        REAL(C_DOUBLE) :: u_a
        REAL(C_DOUBLE) :: u_b

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO i=1, MIN(xn, yn)
            t_a = -(x(i) - a) / q
            t_b =  (x(i) - b) / q

            u_a = 0
            u_b = 0
            DO j=1, FLOOR(k)
                u_a = u_a + t_a**(2*j)
                u_b = u_b + t_b**(2*j)
            END DO

            t_a = 1 - (u_a / FLOOR(k))
            t_b = 1 - (u_b / FLOOR(k))

            t_a = MERGE(a, -q * t_a, x(i) < a)
            t_b = MERGE(b,  q * t_b, x(i) > b)

            y(i) = MERGE(t_a, t_b, x(i) < p)
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_EVEN_POWER_LIMITER

    INTEGER(C_INT) FUNCTION &
        LACLAMP_EVEN_POWER_FOLDOVER(cx, xn, cy, yn, a, b, k) RESULT(code) &
        BIND(C, NAME="LACLAMP_EVEN_POWER_FOLDOVER")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: j
        INTEGER(8)                         :: i
        REAL(C_DOUBLE) :: t_a
        REAL(C_DOUBLE) :: t_b
        REAL(C_DOUBLE) :: u_a
        REAL(C_DOUBLE) :: u_b

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            t_a = -(x(i) - a) / q
            t_b =  (x(i) - b) / q

            u_a = 0
            u_b = 0
            DO j=1, FLOOR(k)
                u_a = u_a + t_a**(2*j)
                u_b = u_b + t_b**(2*j)
            END DO

            t_a = 1 - (u_a / FLOOR(k))
            t_b = 1 - (u_b / FLOOR(k))

            t_a = -q * t_a
            t_b =  q * t_b

            y(i) = MERGE(t_a, t_b, x(i) < p)
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_EVEN_POWER_FOLDOVER

    INTEGER(C_INT) FUNCTION LACLAMP_NROOT(cx, xn, cy, yn, a, b, k) RESULT(code)&
        BIND(C, NAME="LACLAMP_NROOT")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t
                t = (x(i) - p) / q
                t = SIGN(1.0_C_DOUBLE,t) * ABS(t)**(1 / k)
                y(i) = q * t + p
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_NROOT

    INTEGER(C_INT) FUNCTION LACLAMP_ROOT_SINE(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_ROOT_SINE")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t
                t = (x(i) - p) / q
                t = SIN((0.5*PI) * SQRT(ABS(t**k))) * SIGN(1.0_C_DOUBLE, t)
                y(i) = q * t + p
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_ROOT_SINE

    INTEGER(C_INT) FUNCTION LACLAMP_SOFT_ABS(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_SOFT_ABS")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        REAL(C_DOUBLE)                     :: k_r
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)
        k_r = SQRT(1 + k**2) - k

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t
                t = (x(i) - p) / q
                t = (SQRT(t**2 + k**2) - k) / k_r
                y(i) = q * t + p
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_SOFT_ABS

    INTEGER(C_INT) FUNCTION LACLAMP_HARD_GAUSS_1(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_HARD_GAUSS_1")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        REAL(C_DOUBLE)                     :: b_a
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)
        b_a = b - a

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t
                t = MAX(MIN(a + 2*q * ((x(i) - a) / b_a) ,b), a)
                y(i) = t + q * EXP(-(k * ((x(i) - p) / q))**2) 
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_HARD_GAUSS_1

    INTEGER(C_INT) FUNCTION LACLAMP_HARD_GAUSS_2(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_HARD_GAUSS_2")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        REAL(C_DOUBLE)                     :: b_a
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)
        b_a = b - a

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t
                t = MAX(MIN(a + 2*q * ((x(i) - a) / b_a) ,b), a)
                y(i) = t * (1 + q * EXP(-(k * ((x(i) - p) / q))**2)) 
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_HARD_GAUSS_2

    INTEGER(C_INT) FUNCTION LACLAMP_BIT_CRUSH(cx, xn, cy, yn, a, b, k) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_BIT_CRUSH")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        REAL(C_DOUBLE)                     :: n
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)
        n = (2.0_C_DOUBLE)**(k - 1.0)

        DO CONCURRENT (i=1:MIN(xn, yn))
            BLOCK
                REAL(C_DOUBLE) :: t
                t = (x(i) - p) / q
                t = FLOOR(t * n) / n
                y(i) = MIN(MAX(q * t + p, a), b)
            END BLOCK
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_BIT_CRUSH

    INTEGER(C_INT) FUNCTION &
        LACLAMP_TRANSIENT_LIMITER(cx, xn, cy, yn, a, b, k, y0, h) &
        RESULT(code) &
        BIND(C, NAME="LACLAMP_TRANSIENT_LIMITER")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cx
        TYPE(C_PTR), VALUE, INTENT(IN)     :: cy
        INTEGER(C_SIZE_T), INTENT(IN)      :: xn
        INTEGER(C_SIZE_T), INTENT(IN)      :: yn
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: x(:)
        REAL(C_DOUBLE), POINTER, CONTIGUOUS  :: y(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: a
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: b
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: k
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: y0
        REAL(C_DOUBLE), VALUE, INTENT(IN)  :: h
        REAL(C_DOUBLE)                     :: p
        REAL(C_DOUBLE)                     :: q
        REAL(C_DOUBLE)                     :: dx
        REAL(C_DOUBLE)                     :: dx_p
        INTEGER(8)                         :: i

        CALL C_F_POINTER(cx, x, SHAPE=[xn])
        CALL C_F_POINTER(cy, y, SHAPE=[yn])

        p = MIDPOINT(a, b)
        q = MIDSIZE(a, b)

        DO i=1,MIN(xn, yn)
            IF(i > 1) THEN
                dx = (x(i) - y(i-1)) / h
                dx_p = q * TANH(k * (dx - p) / q) + p
                y(i) = y(i-1) + h * dx_p
            ELSE
                y(i) = y0
            END IF
        END DO

        code = 0
        RETURN
    END FUNCTION LACLAMP_TRANSIENT_LIMITER

END MODULE CLAMP_FUNCS
