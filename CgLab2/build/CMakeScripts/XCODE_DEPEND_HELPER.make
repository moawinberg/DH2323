# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.SecondLab.Debug:
/Users/moa/Desktop/dh2323/CgLab2/build/Debug/SecondLab:
	/bin/rm -f /Users/moa/Desktop/dh2323/CgLab2/build/Debug/SecondLab


PostBuild.SecondLab.Release:
/Users/moa/Desktop/dh2323/CgLab2/build/Release/SecondLab:
	/bin/rm -f /Users/moa/Desktop/dh2323/CgLab2/build/Release/SecondLab


PostBuild.SecondLab.MinSizeRel:
/Users/moa/Desktop/dh2323/CgLab2/build/MinSizeRel/SecondLab:
	/bin/rm -f /Users/moa/Desktop/dh2323/CgLab2/build/MinSizeRel/SecondLab


PostBuild.SecondLab.RelWithDebInfo:
/Users/moa/Desktop/dh2323/CgLab2/build/RelWithDebInfo/SecondLab:
	/bin/rm -f /Users/moa/Desktop/dh2323/CgLab2/build/RelWithDebInfo/SecondLab




# For each target create a dummy ruleso the target does not have to exist
