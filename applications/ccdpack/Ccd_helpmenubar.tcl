#+
#  Name:
#     Ccd::menubarhelp

#  Type of Module:
#     [incr Tcl] class

#  Purpose:
#     Defines the "class of menubar with Help".

#  Description:
#     This class is an extension of Ccd::menubar that also performs the
#     standard configuration of the Help system. It is NOT a general
#     widget as it contains CCDPACK specific information.

#  Invocations:
#
#        Ccd::menubarhelp window [-option value]...
#
#     This command create an instance of a menubar with help and returns a
#     command "window" for manipulating it via the methods and
#     configuration options described below. Configuration options may
#     be appended to the command.
#
#        window configure -configuration_options value
#
#     Applies any of the configuration options (after the widget
#     instance has been created).
#
#        window method arguments
#
#     Performs the given method on this widget.

#  Configuration options:
#     See Ccd::menubar.

#  Methods:
#     constructor [-option value]...
#        This method is invoked automatically by the class command and
#	 creates the "class " widget with a default configuration,
#	 except when overridden by command line options.
#     destructor
#        Destroys the "class" instance, invoked by the "delete" method.
#     configure [-option value]...
#        Activates the configuration options. If no configuration value
#	 is given then the current value of any known option is returned
#	 in a form similar (but not identical to) the Tk widget command.
#     sethelpitem item document label
#        Sets the help file to associate with a help item. Item is the
#        name of the help label. "document" the name of the document to
#        be read (for instance, ccdpack or sun139) and label the xlabel
#        in the HTML document to be read. The document index file is
#        scanned to locate the appropriate part. All documents are
#        assumed to be located under the directories stored in the
#        global variable CCDstarhtml.
#     showhelpitem name
#        Displays any help available for the Help menu item labelled
#        "$name".

#  Inheritance:
#     This class inherits Ccd::menubar and its methods and configuration
#     options, which are not directly occluded by those specified here.

#  Authors:
#     PDRAPER: Peter Draper (STARLINK - Durham University)
#     MBT: Mark Taylor (STARLINK)
#     {enter_new_authors_here}

#  History:
#     21--MAR-1995 (PDRAPER):
#     	 Original version.
#     4-MAY-1995 (PDRAPER):
#        Started move to Tk4. Commented out ::rename in destructor, no
#        longer needed.
#     12-MAY-2000 (MBT):
#        Upgraded to Tcl8.
#     27-JAN-2006 (PDRAPER):
#        Updated to itcl::class syntax.
#     {enter_further_changes_here}

#-

   itcl::class Ccd::helpmenubar {

#  Inheritances:
      inherit Ccd::menubar

#.

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#  Construction creates a instance of the Ccd::menubarhelp class and
#  configure it with the default and command-line options.
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      constructor { args } {
         if { $args != {} } {
            eval Ccd::menubar::constructor $args
         }
      } {
         eval configure $args

#  Add standard help commands.
	 if { ! $standard } {
	    addbutton Help 0
	 }

#  Add the basic items to the Help and add commands to invoke
#  them. "On Window" isn't generic.
         addcommand Help {On Context} "$Oldthis showhelpitem {On Context}"
         addcommand Help {On Window}  "$Oldthis showhelpitem {On Window}"
         addcommand Help {On Version} "$Oldthis showhelpitem {On Version}"
         addcommand Help {On CCDPACK} "$Oldthis showhelpitem {On CCDPACK}"
         sethelpitem {On Context} ccdpack OnContext
         sethelpitem {On Version} ccdpack OnVersion
         sethelpitem {On CCDPACK} sun139 {}

#  Add help on the Help menu.
         sethelp Help ccdpack OnHelp

#  And on the File menu.
         if { [info exists Buttons(File)] } { 
            sethelp File ccdpack OnFile
         }
      }

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#  Methods.
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#  Method for assigning context help to a button. This assigns to a
#  pseudo widget .mlabel as need to distinguish between these not just
#  set help for menu as whole. Care needs to be taken not to winfo this!
      method sethelpitem {name docname label} {
         if { [ info exists Buttons(Help) ] } {
            set menulabel $Buttonmenus(Help)$name
            Ccd::base::sethelp $menulabel $docname $label
	 }
      }

#  Invoke help method. This differs from the base class method in that
#  the help is invoked by name rather than position.
      method showhelpitem {name} {
         if { [ info exists Buttons(Help) ] } {
            set menulabel $Buttonmenus(Help)$name
	    if { [info exists helpinfo($menulabel)] } {

#  Search for help and issue command.
	       set docname [CCDLocateHelpFile \
			       [lindex $helpinfo($menulabel) 0]\
			       [lindex $helpinfo($menulabel) 1]]
	       if { "$docname" != "" } {
		  CCDShowHelp $docname
	       } else {
                  CCDIssueInfo "No help available for item \"$name\""
	       }
	    }
	 }
      }

#  End of class defintion.
   }


# $Id$
