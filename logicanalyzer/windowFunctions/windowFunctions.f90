MODULE WINDOW_FUNCTIONS
    USE, INTRINSIC :: ISO_C_BINDING
    IMPLICIT NONE

    REAL(C_DOUBLE), PARAMETER :: PI = ACOS(-1.0)
    CONTAINS

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_RECTANGULAR(cw, N, d, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_RECTANGULAR")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: d
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        REAL(C_DOUBLE)                          :: l1
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        l1 = N * (0.5 - d)

        !$omp parallel do default(none) &
        !$omp shared(w, l1, k, N) &
        !$omp private(i)
        DO i=1,N
            w(i) = MERGE(1.0_C_DOUBLE, 0.0_C_DOUBLE, ABS(i - l1) > k) 
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LAWINDOW_RECTANGULAR

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_TRIANGULAR(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_TRIANGULAR")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        REAL(C_DOUBLE)                          :: l1
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        l1 = N * (0.5_C_DOUBLE + k)

        !$omp parallel do default(none) &
        !$omp shared(w, l1, N) &
        !$omp private(i)
        DO i=1,N
            w(i) = MERGE(i / l1, 1 - (i - l1) / (N - l1), i < l1)
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LAWINDOW_TRIANGULAR

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_WELCH(cw, N)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_WELCH")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE)                          :: n_f
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        n_f = N

        !$omp parallel do default(none) &
        !$omp shared(w, n_f) &
        !$omp private(i)
        DO i=1,N
            w(i) = ((4 * (i-1)) / n_f) * (1 - (i-1) / n_f)
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LAWINDOW_WELCH

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_HANN(cw, N)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_HANN")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE)                          :: n_f
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        n_f = N

        !$omp parallel do default(none) &
        !$omp shared(w, n_f) &
        !$omp private(i)
        DO i=1,N
            w(i) = (1 - COS(2 * PI * (i-1) / n_f)) * 0.5
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LAWINDOW_HANN

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_HAMMING(cw, N)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_HAMMING")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE)                          :: n_f
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        n_f = N

        !$omp parallel do default(none) &
        !$omp shared(w, n_f) &
        !$omp private(i)
        DO i=1,N
            w(i) = 0.54 - 0.46 * COS(2 * PI * (i-1) / n_f)
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LAWINDOW_HAMMING

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_TRAPZ(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_TRAPZ")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        REAL(C_DOUBLE)                          :: l1,l2
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        l1 = N * (0.5_C_DOUBLE + k)
        l2 = N - l1

        !$omp parallel do default(none) &
        !$omp shared(w, l1, l2, k) &
        !$omp private(i)
        DO i=1,N
            w(i) = MERGE(1.0_C_DOUBLE, 1 - (i - 1 - l1) / l2, ABS(i - l1) < k)
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LAWINDOW_TRAPZ

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_CIRCULAR(cw, N)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_CIRCULAR")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE)                          :: n_f
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        n_f = N

        !$omp parallel do default(none) &
        !$omp shared(w, n_f, N) &
        !$omp private(i)
        DO i=1,N
            w(i) = 2 * SQRT((i-1.0_C_DOUBLE) * ((N - 1)**2 - i)) / (n_f - 1)
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LAWINDOW_CIRCULAR

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_SINC(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_SINC")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        REAL(C_DOUBLE)                          :: n_f
        REAL(C_DOUBLE)                          :: t
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        n_f = N

        !$omp parallel do default(none) &
        !$omp shared(w, n_f, N, k) &
        !$omp private(i, t)
        DO i=1,N
            t = k * (i - n_f * 0.5)
            w(i) = MERGE(1.0_C_DOUBLE, SIN(t) / t, ABS(t) < 1e-10)
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LAWINDOW_SINC

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_IMPULSE(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_IMPULSE")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        INTEGER(8)                              :: j
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        i = 1
        j = FLOOR(i * (0.5 + k))

        DO CONCURRENT (i=1:N)
            w(i) = MERGE(1.0, 0.0, i == j) 
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_IMPULSE

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_BLACKMAN(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_BLACKMAN")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        REAL(C_DOUBLE)                          :: n_f
        REAL(C_DOUBLE)                          :: a0, a1, a2
        REAL(C_DOUBLE)                          :: theta
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        a0 = (1.0 - k)
        a1 = 0.5_C_DOUBLE
        a2 = k * 0.5
        n_f = N

        !$omp parallel do default(none) &
        !$omp shared(w, n_f, a0, a1, a2) &
        !$omp private(i, theta)
        DO i=1,N
            theta = 2.0 * PI * (i-1) / n_f
            w(i) = a0 - a1*COS(theta) + a2*COS(2*theta)
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LAWINDOW_BLACKMAN

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_BLACKMAN_HARRIS(cw, N)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_BLACKMAN_HARRIS")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE)                          :: n_f
        REAL(C_DOUBLE)                          :: a0, a1, a2, a3
        REAL(C_DOUBLE)                          :: theta
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        a0 = 0.35878
        a1 = 0.48829
        a2 = 0.14128
        a3 = 0.01668
        n_f = N

        !$omp parallel do default(none) &
        !$omp shared(w, n_f, a0, a1, a2, a3) &
        !$omp private(i, theta)
        DO i=1,N
            theta = 2.0 * PI * (i-1) / n_f
            w(i) = a0 - a1*COS(theta) + a2*COS(2*theta) - a3*COS(3*theta)
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LAWINDOW_BLACKMAN_HARRIS

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_KAISER(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_KAISER")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_KAISER

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_GAUSSIAN(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_GAUSSIAN")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        REAL(C_DOUBLE)                          :: n_2
        REAL(C_DOUBLE)                          :: sigma
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        n_2 = N / 2
        sigma = n_2 * k

        !$omp parallel do default(none) &
        !$omp shared(w, n_2, sigma) &
        !$omp private(i)
        DO i=1,N
            w(i) = EXP(-0.5 * ((i-n_2) / (sigma))**2)
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LAWINDOW_GAUSSIAN

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_FLATTOP(cw, N)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_FLATTOP")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE)                          :: n_f
        REAL(C_DOUBLE)                          :: a0, a1, a2, a3, a4
        REAL(C_DOUBLE)                          :: theta
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        a0 = 0.21557895
        a1 = 0.41663158
        a2 = 0.277263158
        a3 = 0.083578947
        a4 = 0.006947368
        n_f = N

        !$omp parallel do default(none) &
        !$omp shared(w, n_f, a0, a1, a2, a3, a4) &
        !$omp private(i, theta)
        DO i=1,N
            theta = 2.0 * PI * (i-1) / n_f
            w(i) = a0 - a1*COS(theta) + a2*COS(2*theta) - a3*COS(3*theta) + a4*COS(4*theta)
        END DO
        !$omp end parallel do

        code = 0
        RETURN
    END FUNCTION LAWINDOW_FLATTOP

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_PARZEN(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_PARZEN")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_PARZEN

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_COSINE_SUM(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_COSINE_SUM")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_COSINE_SUM

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_BLACKMAN_NUTALL(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_BLACKMAN_NUTALL")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_BLACKMAN_NUTALL

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_NUTALL(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_NUTALL")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_NUTALL

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_SINE_POWER(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_SINE_POWER")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_SINE_POWER

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_APPROX_GAUSSIAN(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_APPROX_GAUSSIAN")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_APPROX_GAUSSIAN

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_TUKEY(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_TUKEY")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_TUKEY

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_PLANCK_TAPER(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_PLANCK_TAPER")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_PLANCK_TAPER

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_POISSON(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_POISSON")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_POISSON

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_LANCZOS(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_LANCZOS")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k 
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_LANCZOS

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_NOISE(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_NOISE")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k 
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_NOISE

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_LOGISTICAL(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_LOGISTICAL")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_LOGISTICAL

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_LOGISTICAL_2(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_LOGISTICAL_2")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_LOGISTICAL_2

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_DAMPED(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_DAMPED")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_DAMPED

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_GAUSSINE(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_GAUSSINE")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_GAUSSINE

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_POLY_CHEBYSCHEV(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_POLY_CHEBYSCHEV")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_POLY_CHEBYSCHEV

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_SMOOTH_TRAPZ(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_SMOOTH_TRAPZ")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE *  k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_SMOOTH_TRAPZ

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_ROOT_CHEBYSCHEV_SMOOTH(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_ROOT_CHEBYSCHEV_SMOOTH")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_ROOT_CHEBYSCHEV_SMOOTH

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_COMPACT_SINE(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_COMPACT_SINE")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_COMPACT_SINE

    INTEGER(C_INT) &
        FUNCTION LAWINDOW_SMOOTH_BOXCAR(cw, N, k)&
        RESULT(code) &

        BIND(C, NAME="LAWINDOW_SMOOTH_BOXCAR")
        USE, INTRINSIC :: ISO_C_BINDING
        TYPE(C_PTR), VALUE, INTENT(IN)          :: cw
        INTEGER(C_SIZE_T), INTENT(IN)           :: N
        REAL(C_DOUBLE), POINTER, CONTIGUOUS     :: w(:)
        REAL(C_DOUBLE), VALUE, INTENT(IN)       :: k
        INTEGER(8)                              :: i
        CALL C_F_POINTER(cw, w, SHAPE=[N])

        DO CONCURRENT (i=1:N)
            w(i) = 1.0_C_DOUBLE * k
        END DO

        code = 0
        RETURN
    END FUNCTION LAWINDOW_SMOOTH_BOXCAR

END MODULE WINDOW_FUNCTIONS
