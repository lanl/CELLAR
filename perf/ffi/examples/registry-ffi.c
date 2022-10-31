#include <assert.h>
#include <perf-ffi-registry.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

// Most applications will declare a global variable for their timer registry, and may declare
// multiple timer registries for different domains.
//
// A `eap_perf_timer_registry_t` can be declared in smaller scopes if desired.
eap_perf_timer_registry_t *timer_registry = NULL;

// Forward declaring our computations.
void foo();
void bar();
void zap();

void foo() {
    // "foo timer" starts when 'push_timer' is called, and stops when
    // 'pop_timer' is called.
    //
    // Since "foo timer" is the outermost timer, its time will be recorded, whereas
    // the inner times will not be tracked.
    eap_perf_timer_handle_t foo_timer = EAP_PERF_TIMER_HANDLE_NULL;
    eap_perf_timer_registry_insert_or_lookup_timer(timer_registry, "foo timer", &foo_timer);

    eap_perf_timer_registry_push_timer(timer_registry, foo_timer);

    bar();

    eap_perf_timer_registry_pop_timer(timer_registry, NULL);
}

void bar() {
    eap_perf_timer_handle_t bar_timer = EAP_PERF_TIMER_HANDLE_NULL;
    eap_perf_timer_registry_insert_or_lookup_timer(timer_registry, "bar timer", &bar_timer);

    eap_perf_timer_registry_push_timer(timer_registry, bar_timer);

    zap();

    eap_perf_timer_handle_t popped_timer = EAP_PERF_TIMER_HANDLE_NULL;
    eap_perf_timer_registry_pop_timer(timer_registry, &popped_timer);

    // 'TimerRegistry<Clock>::pop_timer' returns the 'TimerHandle' of the top timer in the stack.
    assert(popped_timer == bar_timer);
}

void zap() {
    // Timer handles are valid for the lifetime of a timer registry, and so they can be stored
    // across timer runs.
    static eap_perf_timer_handle_t zap_timer = EAP_PERF_TIMER_HANDLE_NULL;
    if (zap_timer == EAP_PERF_TIMER_HANDLE_NULL) {
        eap_perf_timer_registry_insert_or_lookup_timer(timer_registry, "zap timer", &zap_timer);
    }

    eap_perf_timer_registry_push_timer(timer_registry, zap_timer);
    eap_perf_timer_registry_pop_timer(timer_registry, NULL);
}

int main() {
    eap_perf_timer_registry_create(&timer_registry);

    // Run our computation many times.
    for (int i = 0; i < 1000; i++) {
        foo();
    }

    // Run an internal computation, too
    for (int i = 0; i < 1000; i++) {
        bar();
    }

    size_t num_timers;
    eap_perf_timer_registry_get_num_timers(timer_registry, &num_timers);

    printf("Num Timers: %zu\n", num_timers);

    // Dump timer information
    eap_perf_timer_handle_t handle;
    eap_perf_timer_registry_create_timer_iterator(timer_registry, &handle);

    while (eap_perf_timer_registry_next_timer(timer_registry, &handle)) {
        size_t timer_name_length_no_z;
        eap_perf_timer_registry_get_timer_name_length(
            timer_registry, handle, &timer_name_length_no_z);

        char *timer_name = (char *)malloc((timer_name_length_no_z + 1) * sizeof(char));
        eap_perf_timer_registry_get_timer_name(
            timer_registry, handle, timer_name, timer_name_length_no_z + 1);

        eap_perf_timer_statistics_t stats;
        eap_perf_timer_registry_get_statistics(timer_registry, handle, &stats);

        printf("Timer: %s, count = %zi, sum_time = %" PRIi64 " ns",
               timer_name,
               stats.timer_count,
               stats.sum_time);

        if (stats.avg_time >= 0) {
            printf(", avg_time = %" PRIi64 " ns", stats.avg_time);
        }

        printf("\n");
    }

    eap_perf_timer_registry_free(timer_registry);
}