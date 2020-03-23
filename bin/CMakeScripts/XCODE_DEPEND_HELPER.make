# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.FirstLab.Debug:
/Users/moa/Downloads/CgLab1/bin/Debug/FirstLab:
	/bin/rm -f /Users/moa/Downloads/CgLab1/bin/Debug/FirstLab


PostBuild.FirstLab.Release:
/Users/moa/Downloads/CgLab1/bin/Release/FirstLab:
	/bin/rm -f /Users/moa/Downloads/CgLab1/bin/Release/FirstLab


PostBuild.FirstLab.MinSizeRel:
/Users/moa/Downloads/CgLab1/bin/MinSizeRel/FirstLab:
	/bin/rm -f /Users/moa/Downloads/CgLab1/bin/MinSizeRel/FirstLab


PostBuild.FirstLab.RelWithDebInfo:
/Users/moa/Downloads/CgLab1/bin/RelWithDebInfo/FirstLab:
	/bin/rm -f /Users/moa/Downloads/CgLab1/bin/RelWithDebInfo/FirstLab




# For each target create a dummy ruleso the target does not have to exist
