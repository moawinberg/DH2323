# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.FirstLab.Debug:
/Users/moa/Desktop/dh2323/CgLab1/build/Debug/FirstLab:
	/bin/rm -f /Users/moa/Desktop/dh2323/CgLab1/build/Debug/FirstLab


PostBuild.FirstLab.Release:
/Users/moa/Desktop/dh2323/CgLab1/build/Release/FirstLab:
	/bin/rm -f /Users/moa/Desktop/dh2323/CgLab1/build/Release/FirstLab


PostBuild.FirstLab.MinSizeRel:
/Users/moa/Desktop/dh2323/CgLab1/build/MinSizeRel/FirstLab:
	/bin/rm -f /Users/moa/Desktop/dh2323/CgLab1/build/MinSizeRel/FirstLab


PostBuild.FirstLab.RelWithDebInfo:
/Users/moa/Desktop/dh2323/CgLab1/build/RelWithDebInfo/FirstLab:
	/bin/rm -f /Users/moa/Desktop/dh2323/CgLab1/build/RelWithDebInfo/FirstLab




# For each target create a dummy ruleso the target does not have to exist
