#ifndef CHRONOMETER_HPP_
#define CHRONOMETER_HPP_

#include <stdexcept>
#include <time.h>

namespace benchmark {

struct Clock {
    enum Type {
        Thread, Process,
    };
};

//! Execution time chronometer.
class Chrono {
private:
    bool stopped;
    timespec start_time; /**< Chronometer's start time. */
    timespec total_time; /**< Cumulative chronometer's running time. */
    int clock_id;        /**< Chronometer underlying clock id. */

public:
    //! \brief Default chronometer's constructor.
    //!
    //! Build a chronometer mesuring the process execution time.
    Chrono();

    //! \brief Chronometer's constructor.
    //! \param clock The clock type used by the chronometer.
    //!
    //! Build a chronometer, specifying the underlying clock. The underlying
    //! chronometer's clock is used to determine whether the chronometer
    //! will count time for the current thread or for the whole process.
    Chrono(Clock::Type);

    //! \brief Start chronometer.
    void start() throw(std::runtime_error);

    //! \brief Stop chronometer.
    //! \post The elapsed process or thread execution time since last call to
    //! the start method is added to the chronometer's total time.
    void stop() throw(std::runtime_error);

    //! \brief Reset chronometer's time.
    //! \post The chronometer's total time is set to zero.
    void reset();

    //! \brief Retrieve chronometer's second count.
    //! \return The second count.
    long long nsec() const throw (std::logic_error);

    //! \brief Retrieve chronometer's nanosecond count.
    //! \return The nanosecond count.
    long long sec() const throw (std::logic_error);
};

} // namespace benchmark

#endif /* CHRONOMETER_HPP_ */

