# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.project.Debug:
/Users/moa/Desktop/dh2323/project/build/Debug/project:
	/bin/rm -f /Users/moa/Desktop/dh2323/project/build/Debug/project


PostBuild.project.Release:
/Users/moa/Desktop/dh2323/project/build/Release/project:
	/bin/rm -f /Users/moa/Desktop/dh2323/project/build/Release/project


PostBuild.project.MinSizeRel:
/Users/moa/Desktop/dh2323/project/build/MinSizeRel/project:
	/bin/rm -f /Users/moa/Desktop/dh2323/project/build/MinSizeRel/project


PostBuild.project.RelWithDebInfo:
/Users/moa/Desktop/dh2323/project/build/RelWithDebInfo/project:
	/bin/rm -f /Users/moa/Desktop/dh2323/project/build/RelWithDebInfo/project




# For each target create a dummy ruleso the target does not have to exist
