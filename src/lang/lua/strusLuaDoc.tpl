comment --
template main ENDHTML {{ }}
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html lang="en">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
	<title>Strus Lua interface documentation</title>
	<link rel="stylesheet" href="ldoc.css" type="text/css" />
</head>
<body onload="initLoad()">
<script type="text/javascript">
var classSelection = "1";
var memberSelection = "1_1";
var lastNavTable = {};

function showClassNav( id) {
	var navid = 'nav_' + id;
	var navlistid = 'list_' + id;
	var titleid = 'title_' + id;
	var descriptionid = 'description_' + id;
	var allElements = document.getElementsByTagName("*");
	for(var i=0; i < allElements.length; i++)
	{
		if (allElements[i].className === "navclass")
		{
			if (allElements[i].id === navid) {
				allElements[i].style.background = "#e8f8e8";
				allElements[i].style.fontWeight = 'bold';
				allElements[i].style.display = 'block';
			}
			else
			{
				allElements[i].style.fontWeight = 'normal';
				allElements[i].style.background = "#ecffe6";
			}
		}
		else if (allElements[i].className === "navmemberlist")
		{
			if (allElements[i].id === navlistid) {
				allElements[i].style.background = "#e8f8e8";
				allElements[i].style.display = 'block';
			} else {
				allElements[i].style.background = "#ecffe6";
				allElements[i].style.display = 'none';
			}
		}
		else if (allElements[i].className === "nav_title_memberlist")
		{
			if (allElements[i].id === titleid) {
				allElements[i].style.display = 'block';
			} else {
				allElements[i].style.display = 'none';
			}
		}
		else if (allElements[i].className === "classdescription")
		{
			if (allElements[i].id === descriptionid) {
				allElements[i].style.display = 'block';
			} else {
				allElements[i].style.display = 'none';
			}
		}
		else if (allElements[i].className === "memberdescription")
		{
			allElements[i].style.display = 'none';
		}
		else if (allElements[i].className === "navconstructor")
		{
			allElements[i].style.background = "#ecffe6";
		}
		else if (allElements[i].className === "navmethod")
		{
			allElements[i].style.background = "#ecffe6";
		}
	}
	classSelection = id;
}

function showMethod( id) {
	var navid = 'nav_' + id;
	var descriptionid = 'description_' + id;

	var allElements = document.getElementsByTagName("*");
	for(var i=0; i < allElements.length; i++)
	{
		if (allElements[i].className === "navmethod"
		||  allElements[i].className === "navconstructor")
		{
			if (allElements[i].id === navid) {
				allElements[i].style.background = "#e8f8e8";
				allElements[i].style.fontWeight = 'bold';
				allElements[i].style.display = 'block';
			}
			else
			{
				allElements[i].style.fontWeight = 'normal';
				allElements[i].style.background = "#ecffe6";
			}
		}
		else if (allElements[i].className === "memberdescription")
		{
			if (allElements[i].id === descriptionid) {
				allElements[i].style.display = 'block';
			} else {
				allElements[i].style.display = 'none';
			}
		}
	}
	memberSelection = id;
}

function activateClassNav( id) {
	showClassNav( id);
	showMethod( id + "_1");
}

function fillLastNavTable() {
	var allElements = document.getElementsByTagName("*");
	for(var i=0; i < allElements.length; i++)
	{
		if (allElements[i].className === "navmethod" || allElements[i].className === "navclass")
		{
			var idparts = allElements[i].id.split("_");
			if (idparts.length > 2)
			{
				var nav = lastNavTable[ idparts[1]];
				var candidate = parseInt(idparts[2]);
				if (nav == null || nav < candidate) {
					lastNavTable[ idparts[1]] = candidate;
				}
			}
		}
	}
}
function initLoad() {
	activateClassNav( "1");
	fillLastNavTable();
}

function navigateId( id, ofs) {
    if (ofs == 0) return id;
    var idparts = id.split("_");
    var res;
    if (idparts.length == 1) {
        res = (parseInt(idparts[0]) + ofs).toString();
        if (document.getElementById( "nav_" + res) == null) {
            return id;
        } else {
            return res;
        }
    } else {
        var prefix = idparts.slice( 0, idparts.length-1).join("_");
        var newidx = parseInt(idparts[ idparts.length-1]) + ofs;
        res = prefix + "_" + newidx.toString();
        if (document.getElementById( "nav_" + res) != null) {
            return res;
        } else {
            if (ofs > 0) {
                var newprefix = navigateId( prefix, +1);
                if (newprefix == prefix) {
                    return id;
                } else {
                    return newprefix + "_1";
                }
            } else {
                var newprefix = navigateId( prefix, -1);
                var lastnav = lastNavTable[ newprefix];
                if (newprefix == prefix || lastnav == null) {
                    return id;
                } else {
                    return newprefix + "_" + lastnav.toString();
                }
            }
        }
    }
}

document.onkeydown = function(evt) {
    evt = evt || window.event;
    switch (evt.keyCode) {
        case 33://PageUp
            var nav = navigateId( memberSelection, -1);
            var cl = nav.split("_")[0];
            showClassNav( cl);
            showMethod( nav);
            break;
        case 34://PageDown
            var nav = navigateId( memberSelection, +1);
            var cl = nav.split("_")[0];
            showClassNav( cl);
            showMethod( nav);
            break;
    }
}
</script>

<div class="container">
<div class="product">
	<div class="product_logo"> <img class="product_logo_img" src="images/logotype.jpg" alt="Strus"/></div>
	<div class="product_name">{{project}} Lua Bindings</div>
	<div class="product_release">{{release}}</div>
	<div class="product_author">{{author}}</div>
	<div class="product_copyright">{{copyright}}</div>
	<div class="product_license">{{license}}</div>
	<div class="product_description">Lua interface for strus, a set of libraries and programs to build a text search engine</div>
</div> <!-- product -->
<!-- Menu -->
<div class="navigation">
<h1 class="maintitle">strus</h1>

<h2 id="nav_title_classes">Classes</h2>
<div class="navclasslist">
	{{navclasslist}}
</div>
{{navmemberlist}}
</div> <!-- navigation -->

<div class="content">
{{classdescription}}
{{memberdescription}}
</div> <!-- content -->

<div class="about">
<i>generated by papugaDoc ({{project}} {{release}})</i>
</div>

</div>
</body>
</html>
ENDHTML

group constructor method
namespace projectnamespace=project locase
variable project
variable author
variable copyright
variable license
variable release

namespace classname=class
index classidx=class {class}
template navclasslist=class END {{ }}
<div class="navclass" id="nav_{{classidx}}" onclick="activateClassNav('{{classidx}}');">{{classname}}</div>
END

template navmemberlist=class END {{ }}
<h3 class="nav_title_memberlist" id="title_{{classidx}}">{{classname}}</h3>
<div class="navmemberlist" id="list_{{classidx}}">
	{{navmember}}
</div>
END

index methodidx=method      {class} {constructor,method}
index methodidx=constructor {class} {constructor,method}

template navmember=method END {{ }}
<div class="navmethod" id="nav_{{methodidx}}" onclick="showMethod('{{methodidx}}')">{{methodname}}</div>
END
template navmember=constructor END {{ }}
<div class="navconstructor" id="nav_{{methodidx}}" onclick="showMethod('{{methodidx}}')">{{constructorname}}</div>
END

template classdescription=class END {{ }}
<div class="classdescription" id="description_{{classidx}}">
<h2 class="content_title">{{projectnamespace}}_{{classname}}</h2>
<p class="leadtext">
{{description}}
</p>
</div> <!-- classdescription -->
END

template memberdescription=method END {{ }}
<div class="memberdescription" id="description_{{methodidx}}">
<h2 class="content_title">:{{methodname}}</h2>
<p class="text">
{{description}}
</p>
<h4 class="content_subtitle">Parameter</h4>
<div class="parameterlist">{{parameterlist}}</div>
{{callexamples}}
</div> <!-- memberdescription -->
END

template memberdescription=constructor END {{ }}
<div class="memberdescription" id="description_{{methodidx}}">
<h2 class="content_title">.{{constructorname}}</h2>
<p class="text">
{{description}}
</p>
<h4 class="content_subtitle">Parameter</h4>
<div class="parameterlist">{{parameterlist}}</div>
{{callexamples}}
</div> <!-- memberdescription -->
END

template callexamples=constructor END {{ }}
<h4 class="content_subtitle">Examples</h4>
<div class="callexamples">{{examples}}</div>
END

template callexamples=method END {{ }}
<h4 class="content_subtitle">Examples</h4>
<div class="callexamples">{{examples}}</div>
END

template parameterlist=param END {{ }}
<div class="param">
<div class="paramname">{{paramname}}</div>
<div class="paramdescr">{{paramdescr}}</div>
{{paramexamples}}
</div> <!-- param -->
END

empty parameterlist <div class="note">no parameters defined</div>

template paramexamples=param END {{ }}
<div class="paramexamples">{{examples}}</div>
END

template examples=usage END {{ }}
<div class="example">{{example}}</div>
END

variable constructorname=constructor
variable methodname=method
variable description=brief
variable paramname=param[0]
variable paramdescr=param[1:] xmlencode
variable example=usage xmlencode
