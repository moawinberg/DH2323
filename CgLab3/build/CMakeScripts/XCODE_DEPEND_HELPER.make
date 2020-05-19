# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.ThirdLab.Debug:
/Users/moa/Desktop/dh2323/CgLab3/build/Debug/ThirdLab:
	/bin/rm -f /Users/moa/Desktop/dh2323/CgLab3/build/Debug/ThirdLab


PostBuild.ThirdLab.Release:
/Users/moa/Desktop/dh2323/CgLab3/build/Release/ThirdLab:
	/bin/rm -f /Users/moa/Desktop/dh2323/CgLab3/build/Release/ThirdLab


PostBuild.ThirdLab.MinSizeRel:
/Users/moa/Desktop/dh2323/CgLab3/build/MinSizeRel/ThirdLab:
	/bin/rm -f /Users/moa/Desktop/dh2323/CgLab3/build/MinSizeRel/ThirdLab


PostBuild.ThirdLab.RelWithDebInfo:
/Users/moa/Desktop/dh2323/CgLab3/build/RelWithDebInfo/ThirdLab:
	/bin/rm -f /Users/moa/Desktop/dh2323/CgLab3/build/RelWithDebInfo/ThirdLab




# For each target create a dummy ruleso the target does not have to exist
