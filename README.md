RandomForestPlusPlus is a c++ port of a previous .NET project I built for a machine learning competition (the [Kaggle Higgs Boson Machine Learning Challenge](http://www.kaggle.com/c/higgs-boson)). It works, but its true purpose is simply to be a learning project, and to provide goals and structure as I learn c++.

Structure
---------

This is an XCode project, for no better reason than because I'm the sole developer and XCode was what I had handy. Hopefully it will build in other environments without too much hassle, but that hasn't been set up or tested yet.

Targets
-------

* RandomForest++ - Primary executable. Build and link against all libraries. Execute the actual competition code when run.
* Sandbox - Exactly what it sounds like. Nothing useful here, this is just a scratchpad and doesn't get committed to very often.
* BkpClasses - Library target, contains useful classes that I wrote that are not directly related to the Higgs Boson challenge. Could conceivably be used in other projects with minimal modification.
* BkpClassesTests - Executable that runs Google Test unit tests for BkpClasses
* HrfClasses - Classes that were written specifically for the Higgs challenge (hrf = "Higgs Random Forest").
* HrfClassesTests - Executable that runs Google Test unit tests for HrfClasses

Building
--------

At the moment, this can only be built from inside XCode. Once there, simply select one of the executable targets and click the Run button.