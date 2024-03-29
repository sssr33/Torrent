### Notes for Future Maintainers

This was originally imported by @miniksa in January 2020.

The provenance information (where it came from and which commit) is stored in the file `cgmanifest.json` in the same directory as this readme.
Please update the provenance information in that file when ingesting an updated version of the dependent library.
That provenance file is automatically read and inventoried by Microsoft systems to ensure compliance with appropiate governance standards.

## What should be done to update this in the future?

1. Go to chromium/chromium repository on GitHub.
2. Take the entire contents of the base/numerics directory wholesale and drop it in the base/numerics directory here.
3. Don't change anything about it.
4. Validate that the license in the root of the repository didn't change and update it if so. It is sitting in the same directory as this readme. 
   If it changed dramatically, ensure that it is still compatible with our license scheme. Also update the NOTICE file in the root of our repository to declare the third-party usage.
5. Submit the pull.

