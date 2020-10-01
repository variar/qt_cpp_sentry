This is demo project to show how automatic crash reporting may be integrated into C++ application.

Core part of it is [SDK](https://github.com/getsentry/sentry-native) provided by [Sentry](https://sentry.io).
It handles Breakpad/Crashpad linking with the application, collection crash reports and uploading them.
Application has to setup some variable like build version, additional tags etc.
Crash dumps can then be viewed in Sentry dashboard.

Crash report provides information about:

 - operating system: name, version, architecture
 - Qt version
 - modules that were loaded into process: filename, size and hashes for symbols 
 - stacktraces for all running threads in the process

These minidumps do not include content of process memory during the crash. 