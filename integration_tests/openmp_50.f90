program openmp_50
    use omp_lib
    implicit none
    integer :: i=0,n=100
    integer :: array(100)
    call omp_set_num_threads(8)


    do i=1,n
        array(i) = i * 2  ! Initialize the array with some values
    end do

!$OMP PARALLEL SHARED(array)
  !$OMP MASTER
    do i = 1, n
        !$OMP TASK private(i)
            ! print*, omp_get_thread_num()
            print *, "Task started with i =",i
        !$OMP END TASK
    end do
  !$OMP END MASTER
!$OMP END PARALLEL

end program openmp_50
