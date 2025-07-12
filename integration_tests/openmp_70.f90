! Test 7: Runtime Schedule
program test_schedule_runtime
    use omp_lib
    implicit none
    integer, parameter :: n = 100
    integer :: i, tid
    integer :: thread_iterations(0:7) = 0
    character(len=100) :: schedule_env
    logical :: test_passed = .true.
    
    call omp_set_num_threads(4)
    
    print *, "=== RUNTIME Schedule Test ==="
    
    ! Check OMP_SCHEDULE environment variable
    call get_environment_variable("OMP_SCHEDULE", schedule_env)
    if (len_trim(schedule_env) > 0) then
        print *, "OMP_SCHEDULE =", trim(schedule_env)
    else
        print *, "OMP_SCHEDULE not set (will use default)"
    end if
    
    ! Test runtime schedule
    !$omp parallel do schedule(runtime) private(tid)
    do i = 1, n
        tid = omp_get_thread_num()
        !$omp atomic
        thread_iterations(tid) = thread_iterations(tid) + 1
    end do
    !$omp end parallel do
    
    ! Print distribution
    print *, "Thread work distribution with runtime schedule:"
    do i = 0, omp_get_max_threads()-1
        print *, "Thread", i, ":", thread_iterations(i), "iterations"
    end do
    
    ! Test different runtime settings
    print *, ""
    print *, "Testing with different OMP_SCHEDULE settings:"
    
    ! Note: In a real test, you would set these before running the program
    ! Here we just document what should happen
    print *, "- Set OMP_SCHEDULE='static,10' for static chunks of 10"
    print *, "- Set OMP_SCHEDULE='dynamic' for dynamic with chunk 1"
    print *, "- Set OMP_SCHEDULE='guided,5' for guided with min chunk 5"
    
    ! Verify work was distributed
    if (sum(thread_iterations) /= n) then
        print *, "ERROR: Not all iterations were executed!"
        test_passed = .false.
    end if
    
    if (.not. test_passed) then
        error stop "RUNTIME schedule test FAILED!"
    end if
    print *, "RUNTIME schedule test completed"
    
end program test_schedule_runtime

! ============================================

! ! Test 8: Auto Schedule
! program test_schedule_auto
!     use omp_lib
!     implicit none
!     integer, parameter :: n = 1000
!     integer :: i, tid
!     integer :: thread_iterations(0:7) = 0
!     real :: start_time, end_time
!     real :: work_array(n)
    
!     call omp_set_num_threads(4)
    
!     print *, "=== AUTO Schedule Test ==="
!     print *, "AUTO schedule lets compiler choose best schedule"
    
!     ! Initialize with varying workload
!     do i = 1, n
!         if (mod(i, 100) == 0) then
!             work_array(i) = 10.0  ! Heavy iterations
!         else
!             work_array(i) = 1.0   ! Light iterations
!         end if
!     end do
    
!     start_time = omp_get_wtime()
    
!     ! Test auto schedule
!     !$omp parallel do schedule(auto) private(tid)
!     do i = 1, n
!         tid = omp_get_thread_num()
        
!         ! Simulate work based on array
!         call do_work(work_array(i))
        
!         !$omp atomic
!         thread_iterations(tid) = thread_iterations(tid) + 1
!     end do
!     !$omp end parallel do
    
!     end_time = omp_get_wtime()
    
!     ! Print results
!     print *, "Execution time:", end_time - start_time, "seconds"
!     print *, "Thread work distribution:"
!     do i = 0, omp_get_max_threads()-1
!         print *, "Thread", i, ":", thread_iterations(i), "iterations"
!     end do
    
!     ! Auto schedule should handle this efficiently
!     ! We can't predict exact behavior, but work should be distributed
!     if (maxval(thread_iterations(0:3)) > n * 0.9) then
!         print *, "WARNING: AUTO schedule gave most work to one thread"
!     end if
    
!     print *, "AUTO schedule test completed"
    
! contains
!     subroutine do_work(amount)
!         real, intent(in) :: amount
!         integer :: j
!         real :: dummy
        
!         dummy = 0.0
!         do j = 1, int(amount * 100)
!             dummy = dummy + sin(real(j))
!         end do
!     end subroutine do_work
    
! end program test_schedule_auto