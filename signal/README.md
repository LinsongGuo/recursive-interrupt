The program can be used to test the recursion execution of signal handlers.
The program sends multiple signals to itself.
The signal handler in the program takes a long time (about one second) to run.
If one signal handler is still running, the following signals are blocked.
Therefore, the recursive execution of signal handlers is disabled in default.

To resolve this issue, we can unblock the following signals manually by calling `signal_unblock()` function in the program. 
In this way, the singal handlers can be executed recursively.  

To test the effectiveness of manual unblock, there are two modes to run the program: one without unblock, the other with unblock. 

#### Run with `./main` (without manual unblock in signal handler)

The output will be like 

```
signal handler starts
signal handler ends
signal handler starts
signal handler ends
signal handler starts
signal handler ends
...
```
This means that signal handlers don't execute recursively.

#### Run with `./main unblock` (with mannul unblock in signal handler)

The output will be like

```
signal handler starts
signal handler starts
signal handler starts
signal handler starts
signal handler starts
signal handler starts
signal handler starts
signal handler starts
...
Segmentation fault
```

This means that signal handlers execute recursively. (`Segmentation fault` is caused by stack overflow.)
