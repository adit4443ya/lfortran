! Test 1: Static Schedule (default - equal chunks)
program openmp_66
    use omp_lib
    implicit none
    integer, parameter :: n = 100
    integer, parameter :: max_threads = 8
    integer :: i,j
    !$omp parallel do
    do i = 1, n
        !$omp critical
        if(omp_get_thread_num() + 1 == 1) then
            print *, "Thread", omp_get_thread_num() + 1, "first iteration:", i
        end if
        !$omp end critical
    end do
    !$omp end parallel do
end program openmp_66