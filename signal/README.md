The program can be used to test the recursion execution of signal handlers.
The program sends multiple signals to itself.
The signal handler in the program takes a long time (about one second) to run.
If one signal handler is still running, the following signals are blocked.
Therefore, the recursive execution is disabled in default.

To resolve this issue, we can unblock the signals manually. 
In this way, the singal handlers can be executed recursively.  

To test the effectiveness of manual unblock, there are two modes to run the program: one without unblock, the other with unblock. 

#### run with `./main` (without unblock)

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

#### run with `./main unblock` (with unblock)

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
Segmentation fault
```

This means that signal handlers execute recursively. (`Segmentation fault` is caused by stack overflow.)
