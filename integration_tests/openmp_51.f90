program openmp_51
    use omp_lib
    implicit none
    integer :: i=0,n=100
    call omp_set_num_threads(8)

    !$omp parallel
        do i=1,n
            !$omp single
                print *, "Welcome to Single Thread!"
            !$omp end single
        end do
    !$omp end parallel 
end program openmp_51