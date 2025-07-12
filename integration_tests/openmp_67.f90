program test_schedule_guided
    use omp_lib
    implicit none
    integer, parameter :: n = 1000  ! Larger n to see guided behavior
    integer :: i, tid, j
    integer :: chunk_count
    integer :: chunk_size_array(100) = 0
    integer :: chunk_thread(100) = -1
    chunk_count=0
    
    ! Track chunk sizes with guided schedule
    !$omp parallel private(tid)
    !$omp do schedule(guided)
    do i = 1, n
        tid = omp_get_thread_num()
        
        !$omp critical
        if (i == 2) then
            chunk_count = chunk_count + 1
            ! chunk_thread(chunk_count) = tid
            
            ! Find chunk size by looking ahead
            do j = i, n
                if (j == n) then
                    ! chunk_size_array(chunk_count) = j - i + 1
                end if
            end do
        end if
        !$omp end critical
    end do
    !$omp end do
    !$omp end parallel
end program test_schedule_guided