# Introduction

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
 - memory content for stack frames

These minidumps do not include content of heap process memory during the crash. 

# Crash reporting integration at a glance

These steps are required to integrate crash reporting into your C++ application:
 1. Create an account at [Sentry](https://sentry.io). Free plan has enough features
 to get started.
 2. Create a project at Sentry and find DSN string in its settings. Sentry SDK
 will send crash reports to this endpoint.
 3. Add [native Sentry SDK](https://github.com/getsentry/sentry-native) as a git submodule.
 4. Add Sentry directory to CMakeLists.txt. Set Sentry backend to `Crashpad` and transport to `none`.
 5. Add dependencies on sentry and crashpad-client to executable targets.
 6. Setup Sentry SDK at program start.
    1. Create directory for crash reports if needed.
    2. Check that directory for new reports (using crashpad library) and ask user
    confirmation to send any to Sentry.
    3. Initialize Sentry SDK.
 7. Shutdown Sentry SDK at program end.



