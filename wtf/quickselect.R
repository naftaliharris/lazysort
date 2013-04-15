# quickselect.R
# Author: Naftali Harris

qsSimul <- function(n, target) {
    pivot <- sample(n, 1)
    if(target < pivot) {
        return(n + qsSimul(pivot, target))
    }
    else if (target > pivot) {
        return(n + qsSimul(n - pivot, target - pivot))
    }
    else {
        return(n)
    }
}

reps <- 5000
ns <- c(10^(1:8), 16, 32, 64, 88, 128, 196, 256, 512)
times <- sapply(ns, function(n) {mean(replicate(reps, n + qsSimul(n, n / 2))) })

plot(ns, times / ns, log="x")
