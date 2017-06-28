var width  = 800;
var height = 500;
var out_rad = 15;
var pie_rad = out_rad * 2.5;
var force;
var metadata_mode = false;
var points;

document.onmousemove = getMouseCoordinates;

var LOCATION;
var pt;


function dirname (path)
{
    // http://kevin.vanzonneveld.net
    // +   original by: Ozh
    // +   improved by: XoraX (http://www.xorax.info)
    // *     example 1: dirname('/etc/passwd');
    // *     returns 1: '/etc'
    // *     example 2: dirname('c:/Temp/x');
    // *     returns 2: 'c:/Temp'
    // *     example 3: dirname('/dir/test/');
    // *     returns 3: '/dir'
    return path.replace(/\\/g, '/').replace(/\/[^\/]*\/?$/, '');
}


function bring_to_front(el)
{
    // move element "on top of" all others within the same grouping
    el.parentNode.appendChild(el);  
}


function getInnerWidth()
{
    return(window.innerWidth - 40);
}


function getInnerHeight()
{
    return(window.innerHeight - 40);
}


function do_clique()
{
    window.onresize = function (){
        window.location.href = window.location.href
    };
    width           = getInnerWidth();
    height          = getInnerHeight();
    //height          = json.nodes.length * 80;
    //height          = width;
    var in_rad      = 50;
    var color       = d3.scale.category10();
   
    for (i = 0; i < 30; ++i) {
        color(i);
    }
     
    var svg = d3.select("#chart")
    .append("svg")
    .attr("width",  width)
    .attr("height", height)
    .append("g");
    
    var num_nodes = json.nodes.length;
    points    = get_points(num_nodes, 1000, (width / 2), (height / 2));
      
    for (var i in json.nodes)
    {
        json.nodes[i].point = points[i];
    }
      
    var link = svg.selectAll("line.link")
    .data(json.links)
    .enter()
    .append("line")
    .attr("class", "link")
    .attr("x1", function(d) {
        return points[d.source].x;
    })
    .attr("y1", function(d) {
        return points[d.source].y;
    })
    .attr("x2", function(d) {
        return points[d.target].x;
    })
    .attr("y2", function(d) {
        return points[d.target].y;
    })
    .on("mouseover", function (d) 
    {
        var value   = d.value;
        var val_len = value.toString().length;
  
        d3.select(this)
        .transition()
        .style("stroke-width", function(d) {
            return 15;
        })
        .style("stroke",       function(d) {
            return "#000000";
        });
  
        d3.select(this.parentNode)
        .append("rect")
        .attr("x", this.getBBox().x + this.getBBox().width  / 2)
        .attr("y", this.getBBox().y + this.getBBox().height / 2)
        .attr("rx", 5)
        .attr("ry", 5)
        .attr("stroke", "#000000")
        .attr("stroke-width", 2)
        .attr("width",  val_len * 8 + 4)
        .attr("height", 20)
        .attr("fill", "#FFFFFF")
        .attr("pointer-events", "none");
      
        d3.select(this.parentNode)
        .append("text")
        .text(function(d) {
            return value;
        })
        .attr("x", this.getBBox().x + this.getBBox().width  / 2 +  2)
        .attr("y", this.getBBox().y + this.getBBox().height / 2 + 16)
        .attr("font-family", "monospace")
        .attr("fill", "#000000")
        .attr("pointer-events", "none");
    })
    .on("mouseout", function (d) 
    {
        d3.select(this)
        .transition()
        .style("stroke-width", function(d) {
            return 5;
        })
        .style("stroke",       function(d) {
            return "#999999";
        });
      
        this.parentNode.removeChild(this.parentNode.lastChild);
        this.parentNode.removeChild(this.parentNode.lastChild);
      
    })
    .style("stroke-width", function(d) {
        return 5;
    });
      
    var node = svg.selectAll("g.node")
    .data(json.nodes)
    .enter().append("svg:g")
    .attr("class", "node");
      
    node.append("circle")
    .attr("r", out_rad)
    .attr("cx", function(d) {
        return d.point.x;
    })
    .attr("cy", function(d) {
        return d.point.y;
    })
    .style("fill", function(d) {
        return color(d.group);
    })
    .style("stroke", function(d) {
        return get_border_color(d.name);
    })
    .style("stroke-width", function(d) {
        return 2;
    })
    .on("mouseover", function (d)
    {               
        d3.select(this.parentNode)
        .select("text")
        .attr("display", "block");

        var width = this.parentNode.getElementsByTagName("text")[0].getBBox().width;
                 
        d3.select(this)
        .transition()
        .attr("r", (width / 2) + 4);

        bring_to_front(this.parentNode);
    })
    .on("mouseout", function (d)
    {
        d3.select(this)
        .transition()
        .attr("r", out_rad);

        d3.select(this.parentNode).select("text")
        .attr("display", "none");
    });
        
    node.append("text")
    .text(function(d) {
        return d.name.toUpperCase();
    })
    .attr("font-family", "monospace")
    .attr("font-weight", "bold")
    .attr("fill", "#FFFFFF")
    .attr("pointer-events", "none")
    .attr("x", function(d) {
        return d.point.x - ((d.name.length * 16) / 4);
    })
    .attr("y", function(d) {
        return d.point.y;
    })
    .attr("dy", ".31em")
    .on("mouseover", function (d) {
        return;
    })
    .attr("display", "none");
      
    init_svg();
    return;
}


function do_subgraph(use_species)
{
    window.onresize = function () {
        resize_svg()
    };
    width           = getInnerWidth();
    //height          = 5000;
    height          = Math.max(500, json.nodes.length * 20);
    //width           = Math.min(json.nodes.length * 40, getInnerWidth());
    var in_rad      = 50;
    var out_rad     = 15;
    var charge      = -2000;
    var linkDist    = 150;
    var color       = d3.scale.category10();
    var spec_color  = d3.scale.category20();
 
    for (i = 0; i < 30; ++i) 
    { 
        color(i); 
        spec_color(i);
    }
  
    force = d3.layout
    .force()
    .charge(charge)
    .linkDistance(linkDist)
    .size([width, height]);
  
    d3.select("#chart")
    .select("svg")
    .remove();
  
    var svg = d3.select("#chart")
    .append("svg")
    .attr("width", width)
    .attr("height", height)
    .append("g");
  
    var species_map = new Object();
      
    var idx = 0;
    for (var i in json.nodes)
    {
        var species = json.nodes[i].species;
        
        if (! species_map[species])
        {
            species_map[species] = idx;
            ++idx;
        }
    }
      
    force.nodes(json.nodes)
    .links(json.links)
    .start();
  
    var link = svg.selectAll("line.link")
    .data(json.links)
    .enter()
    .append("line")
    .attr("class", "link")
    .on("mouseover", function (d) 
    {
        var value   = d.value;
        var val_len = value.toString().length;
          
        d3.select(this)
        .transition()
        .style("stroke-width", function(d) {
            return 15;
        })
        .style("stroke", function(d) {
            return "#000000";
        });
             
        d3.select(this.parentNode)
        .append("rect")
        .attr("x", this.getBBox().x + this.getBBox().width / 2)
        .attr("y", this.getBBox().y + this.getBBox().height / 2)
        .attr("rx", 5)
        .attr("ry", 5)
        .attr("stroke", "#000000")
        .attr("stroke-width", 2)
        .attr("width", val_len * 8 + 4)
        .attr("height", 20)
        .attr("fill", "#FFFFFF")
        .attr("pointer-events", "none");

        d3.select(this.parentNode)
        .append("text")
        .text(function(d) {
            return value;
        })
        .attr("x", this.getBBox().x + this.getBBox().width  / 2 + 2)
        .attr("y", this.getBBox().y + this.getBBox().height / 2 + 16)
        .attr("font-family", "monospace")
        .attr("fill", "#000000")
        .attr("pointer-events", "none");
                           
    })
    .on("mouseout", function (d) 
    {
        d3.select(this)
        .transition()
        .style("stroke-width", function(d) {
            return 5;
        })
        .style("stroke", function(d) {
            return "#999999";
        });
              
        this.parentNode.removeChild(this.parentNode.lastChild);
        this.parentNode.removeChild(this.parentNode.lastChild);
              
    })
    .style("stroke-width", function(d) {
        return 5;
    });
  
    var node = svg.selectAll("g.node")
    .data(json.nodes)
    .enter().append("svg:g")
    .attr("class", "node")
    .call(force.drag);

    var myarray = new Array();
       
    node.append("circle")
    .attr("r", out_rad)
    .style("fill", function(d) 
    { 
        if (use_species)
        {
            return spec_color(species_map[d.species]);
        }
        return color(d.group); 
    })
    .style("stroke",       function(d) {
        return get_border_color(d.name);
    })
    .style("stroke-width", function(d) {
        return 2;
    })
    .on("click", function (d) 
    {
        if (d.group > 0)
        {
            window.location.href = dirname(location.href) + "/clique." + d.group + ".html";
        }
        else
        {
        //alert(d.name + " is not part of a clique");
        }
    })
    .on("mouseover", function (d)
    {
        var species = d.species;
        
        svg.selectAll("text")
        .transition()
        .attr("display", function (d) 
        {                                   
            if (d.species == species || document.getElementById("nodetextvisiblecheckbox").checked)
            {
                bring_to_front(this);
                return 'block';
            }
            return 'none';
        });

        svg.selectAll("circle")
        .transition()
        .attr("r", function (d) 
        {
            return (d.species == species) ? (d.name.length * 4) + 4  : out_rad;
        });

        d3.select(this.parentNode)
        .select("text")
        .attr("display", "block");

        var width = this.parentNode.getElementsByTagName("text")[0].getBBox().width;

        d3.select(this)
        .transition()
        .attr("r", (width / 2) + 4);

        bring_to_front(this.parentNode);
    })
    .on("mouseout", function (d)
    {
        svg.selectAll("circle")
        .transition()
        .attr("r", function (d) {
            return out_rad;
        });
        
        svg.selectAll("text")
        .transition()
        .attr("display", function (d)
        {
            if (document.getElementById("nodetextvisiblecheckbox").checked) return "block";
            return 'none';
        });
        
        d3.select(this)
        .transition()
        .attr("r", out_rad);

        d3.select(this.parentNode)
        .select("text")
        .attr("display", "none");
    });

    node.append("text")
    .text(function(d) {
        return d.name.toUpperCase();
    })
    .attr("font-family", "monospace")
    .attr("font-weight", "bold")
    .attr("fill", "#FFFFFF")
    .attr("pointer-events", "none")
    .attr("x", function(d) {
        return 20;
    })
    .attr("dy", ".31em")
    .on("mouseover", function (d) {
        return;
    })
    .attr("display", "none");

  
    force.on("tick", function()
    {
        link.attr("x1", function(d) {
            return d.source.x;
        })
        .attr("y1", function(d) {
            return d.source.y;
        })
        .attr("x2", function(d) {
            return d.target.x;
        })
        .attr("y2", function(d) {
            return d.target.y;
        });
  
        node.select("circle")
        .attr("cx", function(d) {
            return d.x = Math.max(pie_rad, Math.min(width  - pie_rad, d.x));
        })
        .attr("cy", function(d) {
            return d.y = Math.max(pie_rad, Math.min(height - pie_rad, d.y));
        });
  
   
        node.selectAll("path")
        .attr("d", function(d) 
        { 
            // when we only want one slice, we have to use two semicircles to color it. 
            // the following bit of code will lower the top semicircle by 1 pixel, to avoid
            // the small gap that appears
            var addition = 0;
            if (d3.select(this).attr("num_slices") == 2 && d3.select(this).attr("slice_num") == 1)
            {
                addition = 1;
            }
	  
            var ret_array = get_slice_path(d3.select(this).attr("num_slices"), 
                d3.select(this).attr("slice_num"), 
                d3.select(this).attr("radius"), 
                Math.max(pie_rad, Math.min(width  - pie_rad, d.x)), 
                Math.max(pie_rad, Math.min(height - pie_rad, d.y)) + addition); 
	  
            d3.select(this).attr("text_x", ret_array[1]);
            d3.select(this).attr("text_y", ret_array[2]);
            return ret_array[0];
        });
	
        node.selectAll("text")
        .attr("x", function(d) 
        { 
            // slice text
            if (d3.select(this).attr("num_slices") > 0)
            {
                var ret_array = get_slice_path(d3.select(this).attr("num_slices"), 
                    d3.select(this).attr("slice_num"),  
                    pie_rad,
                    Math.max(pie_rad, Math.min(width  - pie_rad, d.x)), 
                    Math.max(pie_rad, Math.min(height - pie_rad, d.y)));
	                        
                return d.x + ret_array[1] - 5;
            }

            return d.x - ((d.name.length * 16) / 4); 
        })
        .attr("y", function(d) 
        { 
            // slice text
            if (d3.select(this).attr("num_slices") > 0)
            {
                var ret_array = get_slice_path(d3.select(this).attr("num_slices"), 
                    d3.select(this).attr("slice_num"), 
                    pie_rad,
                    Math.max(pie_rad, Math.min(width  - pie_rad, d.x)), 
                    Math.max(pie_rad, Math.min(height - pie_rad, d.y)));
      
                return d.y + ret_array[2];
            }

            // node name text
            if (metadata_mode) 
            {
                return Math.max(5, d.y - pie_rad - 5);
            }

            return d.y; 
        });

  
        node.attr("x", function(d) {
            return d.x;
        })
        .attr("y", function(d) {
            return d.y;
        });
    });
  
    init_svg();
    return;
}


function get_slice_path(num_slices, slice_num, radius, cx, cy)
{
    //console.log("NS:" + num_slices + " S#:" +  slice_num + " R:" + radius + " Cx:" + cx + " Cy:" + cy);
    num_slices = Number(num_slices);
    slice_num  = Number(slice_num);
    radius     = Number(radius);
    cx         = Number(cx);
    cy         = Number(cy);
   
    var convert  = 0.01745239; 
   
    var degrees1 = slice_num       * (360 / num_slices);
    var degrees2 = (slice_num + 1) * (360 / num_slices);
   
   
    var radians1 = convert * degrees1;
    var radians2 = convert * degrees2;
   
   
    var x1 = (radius * Math.cos(radians1)); // first point on the circle
    var y1 = (radius * Math.sin(radians1)); 
   
    var x2 = (radius * Math.cos(radians2)); // second point on the circle
    var y2 = (radius * Math.sin(radians2)); 
   
    //console.log(slice_num + ":  " + x1 + "," + y1 + "  " + x2 + "," + y2 + " [" + degrees1 + " - " + degrees2 + "]");

    // get the location for the text over this slice
    var text_x = (x1 + x2) / 2;
    var text_y = (y1 + y2) / 2;
    if (num_slices == 2 && slice_num == 0) text_y = radius / 2;                            // when there are only two slices
    if (num_slices == 2 && slice_num == 1) text_y = - (radius / 2);
    if (num_slices == 2 && (Math.abs(x1 + x2) < 0.1 || Math.abs(x1 - x2) < 0.1)) text_x = 0; // when there is only one slice
    if ((text_y - 10) < (0 - radius)) text_y += (radius / 2) - 8;
   
   
    x1 += cx;
    y1 += cy;
    x2 += cx;
    y2 += cy;

    //console.log(slice_num + ":  " + x1 + "," + y1 + "  " + x2 + "," + y2 + " [" + degrees1 + " - " + degrees2 + "]");


    var path = "M" + cx + "," + cy + " L" + x1 + "," + y1 + " A" + radius + "," + radius + " 0 0,1 " + x2 + "," + y2 + " z";
    return [path, text_x, text_y];
}


function color_by_species()
{
    remove_metadata();
    metadata_mode = false;
  
    var spec_color  = d3.scale.category20();
    var species_map = new Object();
    var idx         = 0;

    for (i = 0; i < 30; ++i) {
        spec_color(i);
    }

    for (var i in json.nodes)
    {
        var species = json.nodes[i].species;
          
        if (! species_map[species])
        {
            species_map[species] = idx;
            ++idx;
        }
    }
      
    var svg = d3.select("#chart")
    .select("svg")
    .selectAll("g.node")
    .selectAll("circle")
    .style("fill", function(d) {
        return spec_color(species_map[d.species]);
    });
                
    set_text_color();

    return;
}


function color_by_clique()
{
    remove_metadata();
    metadata_mode = false;
  
    var color = d3.scale.category10();

    for (i = 0; i < 30; ++i) {
        color(i);
    }

    var svg   = d3.select("#chart")
    .select("svg")
    .selectAll("g.node")
    .selectAll("circle")
    .style("fill", function(d) {
        return color(d.group);
    });
                
    set_text_color();

    return;
}


function color_slices_by_metadata(columns)
{      
    var nodes     = delete_paths(); 
    metadata_mode = true;
  
    select_all_nodes().selectAll("text")
    .attr("y", function (d) 
    {
        return Number(d3.select(this.parentNode).select("circle").attr("cy") - pie_rad - 10);
    });
            
    //console.log("coloring using columns:");
    //console.log(columns);
 
    var num_slices = columns.length;
 
    // if there is only one slice, we have to make a circle using to semi-circles
    if (num_slices == 1)
    {
        var color = "blue";
   
        var nodes = select_all_nodes();
    
        // first half of circle
        nodes.append("path")
        .attr("fill", function(d) {
            return get_slice_color(columns[0], d.name);
        })
        .attr("radius", function(d) {
            return (node_has_metadata(d.name)) ? pie_rad : out_rad + 2;
        })
        .attr("num_slices", 2)
        .attr("slice_num", 0)
        .attr("column", columns[0])

        .attr("d", function(d) 
        { 
            if (force) return "M0,0 L0,0 A0,0 0 0,1 0,0 z"; // force layout will draw the path
            var addition = 1;
	  
            var ret_array = get_slice_path(2, 
                0, 
                pie_rad, 
                Math.max(pie_rad, Math.min(width  - pie_rad, d.point.x)), 
                Math.max(pie_rad, Math.min(height - pie_rad, d.point.y)) + addition); 
            return ret_array[0];
        })
        .on("mouseover", function (d)
        {
            if (! is_ratio(d3.select(this).attr("column"))) return;
            var x = d.x;
            var y = d.y;
            if (! force) 
            {
                x = d.point.x;
                y = d.point.y;
            }
	          
            if (node_has_metadata(d.name))
            {
                get_scale(get_normalized(d3.select(this).attr("column"), d.name), x, y, d3.select(this).attr("column"));
            }
        })
        .on("mouseout", function (d)
        {
            if (! is_ratio(d3.select(this).attr("column"))) return;
            //console.log('out');
            document.getElementById('floater').style.display = "none";
        });
         
        nodes = select_all_nodes();
 	   
        // second half of circle
        nodes.append("path")
        .attr("fill", function(d) {
            return get_slice_color(columns[0], d.name);
        })
        .attr("radius", function(d) {
            return (node_has_metadata(d.name)) ? pie_rad : out_rad + 2;
        })
        .attr("num_slices", 2)
        .attr("slice_num", 1)
        .attr("column", columns[0])
        .attr("d", function(d) 
        { 
            if (force) return "M0,0 L0,0 A0,0 0 0,1 0,0 z"; // force layout will draw the path
            var addition = 1;
	  
            var ret_array = get_slice_path(2, 
                1, 
                pie_rad, 
                Math.max(pie_rad, Math.min(width  - pie_rad, d.point.x)), 
                Math.max(pie_rad, Math.min(height - pie_rad, d.point.y)) + addition); 
            return ret_array[0];
        })
        .on("mouseover", function (d)
        {
            if (! is_ratio(d3.select(this).attr("column"))) return;
            var x = d.x;
            var y = d.y;
            if (! force) 
            {
                x = d.point.x;
                y = d.point.y;
            }
	          
            if (node_has_metadata(d.name))
            {
                get_scale(get_normalized(d3.select(this).attr("column"), d.name), x, y, d3.select(this).attr("column"));
            }
        })
        .on("mouseout", function (d)
        {
            if (! is_ratio(d3.select(this).attr("column"))) return;
	          
            //console.log('out');
            document.getElementById('floater').style.display = "none";
        });
         
        nodes.append("text")
        .attr("font-size", 10)
        .attr("fill", "white")
        .attr("x", function(d) 
        { 
            if (force) return 0; // force layout will draw the path
            var addition = 1;
	   	  
            var ret_array = get_slice_path(2, 
                1, 
                pie_rad, 
                Math.max(pie_rad, Math.min(width  - pie_rad, d.point.x)), 
                Math.max(pie_rad, Math.min(height - pie_rad, d.point.y)) + addition); 
	   	  
	   	  
            return ret_array[1] + Number(d3.select(this.parentNode).select("circle").attr("cx"));
        })
        .attr("y", function(d) 
        { 
            if (force) return 0; // force layout will draw the path
            var addition = 1;
	   	  
            var ret_array = get_slice_path(2, 
                1, 
                pie_rad, 
                Math.max(pie_rad, Math.min(width  - pie_rad, d.point.x)), 
                Math.max(pie_rad, Math.min(height - pie_rad, d.point.y)) + addition); 
	   	  
	   	 
            return ret_array[2] + Number(d3.select(this.parentNode).select("circle").attr("cy"));
        })
        .attr("slice_num", 1)
        .attr("num_slices", 2)
        .text(function (d) {
            return get_slice_label(columns[0], d.name);
        });

    }
    else
    {
        for (var i = 0; i < num_slices; ++i)
        {
            var nodes = select_all_nodes();
    
            nodes.append("path")
            .attr("id", function (d) {
                return d.name + "_" + i;
            })
            .attr("fill", function(d) {
                return get_slice_color(columns[i], d.name);
            })
            .attr("stroke", "white")
            .attr("radius", function(d) {
                return (node_has_metadata(d.name)) ? pie_rad : out_rad + 2;
            })
            .attr("num_slices", num_slices)
            .attr("slice_num", i)
            .attr("column", columns[i])
            .attr("d", function(d) 
            { 
                if (force) return "M0,0 L0,0 A0,0 0 0,1 0,0 z"; // force layout will draw the path
                var addition = 1;
	   	  
                var ret_array = get_slice_path(num_slices, 
                    i, 
                    pie_rad, 
                    Math.max(pie_rad, Math.min(width  - pie_rad, d.point.x)), 
                    Math.max(pie_rad, Math.min(height - pie_rad, d.point.y)) + addition); 
	   	  
	   	  
                return ret_array[0];
            })
            .on("mouseover", function (d)
            {
                if (! is_ratio(d3.select(this).attr("column"))) return;
                var x = d.x;
                var y = d.y;
                if (! force) 
                {
                    x = d.point.x;
                    y = d.point.y;
                }
	          
                if (node_has_metadata(d.name))
                {
                    get_scale(get_normalized(d3.select(this).attr("column"), d.name), x, y, d3.select(this).attr("column"));
                }
            })
            .on("mouseout", function (d)
            {
                if (! is_ratio(d3.select(this).attr("column"))) return;
	         
                //console.log('out');
                document.getElementById('floater').style.display = "none";
            });
      
          
            nodes.append("text")
            .attr("font-size", 10)
            .attr("fill", "white")
            .attr("x", function(d) 
            { 
                if (force) return 0; // force layout will draw the path
                var addition = 1;
	   	  
                var ret_array = get_slice_path(num_slices, 
                    i, 
                    pie_rad, 
                    Math.max(pie_rad, Math.min(width  - pie_rad, d.point.x)), 
                    Math.max(pie_rad, Math.min(height - pie_rad, d.point.y)) + addition); 
	   	  
	   	  
                return ret_array[1] + Number(d3.select(this.parentNode).select("circle").attr("cx"));
            })
            .attr("y", function(d) 
            { 
                if (force) return 0; // force layout will draw the path
                var addition = 1;
	   	  
                var ret_array = get_slice_path(num_slices, 
                    i, 
                    pie_rad, 
                    Math.max(pie_rad, Math.min(width  - pie_rad, d.point.x)), 
                    Math.max(pie_rad, Math.min(height - pie_rad, d.point.y)) + addition); 
	   	  
	   	 
                return ret_array[2] + Number(d3.select(this.parentNode).select("circle").attr("cy"));
            })
            .attr("slice_num", i)
            .attr("num_slices", num_slices)
            .text(function (d) {
                return get_slice_label(columns[i], d.name);
            });      
        }
    }
   
  
    select_all_nodes().selectAll("text")
    .attr("display", function (d) 
    { 
        if (! node_has_metadata(d.name))
        {
            return "none";
        }

        // this is a metadata label (not a node label)
        if (d3.select(this).attr("num_slices") > 0)
        {
            if (document.getElementById("textvisiblecheckbox").checked)
            {
                bring_to_front(this); 
                return "block";
            }

            return "none";
        }

        // this is a node label
        bring_to_front(this); 
        return (document.getElementById("nodetextvisiblecheckbox").checked) ? "block" : "none";
    });

  
    if (force)
    {
        force.stop();
        force.resume();
    }
//bring_circles_to_front();
}


function node_has_metadata(name)
{
    var metadata = getMetadata();
    
    for (var i in metadata)
    {
        if (metadata[i][name]) return true;
        return false; 
    }
    return false;
}


function get_slice_label(column, name)
{
    if (!column) return "";
  
    var metadata   = getMetadata();
    var properties = getProperties(); //TODO: get by column
  
    var min  = properties[column]['min'];
    var max  = properties[column]['max'];
    var diff = max - min;
  
    var color_min  = 115;
    var color_max  = 255;
    var color_diff = color_max - color_min;

    if (metadata[column][name] == "null") return "";
    if (metadata[column][name] != null) return format_number(metadata[column][name]);
    return "";
}


function get_normalized(column, name)
{
    var metadata   = getMetadata();
    var properties = getProperties(); //TODO: get by column
  
    var min   = defaultColumns[column]['min'];
    var max   = defaultColumns[column]['max'];
    var ratio = properties[column]['type'];
    var diff = max - min;

    if (metadata[column][name] == null) return "gray";
    var metadata_val = metadata[column][name];
    if (metadata_val == "null") return "white";

    var perc  = (metadata_val - min) / diff;
  
    return Math.floor(perc * 100);
}


function get_slice_color(column, name)
{
    var metadata   = getMetadata();
    var properties = getProperties(); //TODO: get by column
  
    var min   = properties[column]['min'];
    var max   = properties[column]['max'];
    var ratio = properties[column]['type'];
    var diff = max - min;

    if (metadata[column][name] == null) return "gray";
    var metadata_val = metadata[column][name];
    if (metadata_val == "null") return "white";

    var perc  = (metadata_val - min) / diff;
  
    if (ratio.toLowerCase() == 'r')
    {
        var orig_max = defaultColumns[column]['max'];
        var orig_min = defaultColumns[column]['min'];
    
        var max_perc = (max / orig_max) * 100;
        var min_perc = Math.abs(min / orig_min) * 100;
        var zero_loc = 100 - ((orig_max / (orig_max + Math.abs(orig_min))) * 100);
        var i = get_normalized(column, name);
        //console.log(i + ":" +  min_perc + ":" +   max_perc + ":" +  zero_loc)
        perc = get_perc(i, min_perc, max_perc, zero_loc);
        return getColorForPercentage(perc);
    }
                  
    var color_func = d3.scale.category10();
    var col_array = Object.keys(properties);
  
    for (var i in col_array)
    {
        color_func(col_array[i]);
    }
  
    var rgb = d3.rgb(color_func(column));
    if (perc > .5) return rgb.brighter(perc + .5);
    return rgb.darker(perc + .5 );
}


var percentColors = [
{
    pct: 0.0, 
    color: {
        r: 0xff, 
        g: 0x22, 
        b: 0
    }
},
{
    pct: 0.5, 
    color: {
        r: 0xff, 
        g: 0xff, 
        b: 0
    }
},
{
    pct: 1.0, 
    color: {
        r: 0x22, 
        g: 0xff, 
        b: 0
    }
} ];

var getColorForPercentage = function(pct) {

    if (pct <= 0) pct = 0.01;
    if (pct >= 1) pct = 0.99;

    for (var i = 0; i < percentColors.length; i++) {
        if (pct <= percentColors[i].pct) {
            var lower = percentColors[i - 1];
            var upper = percentColors[i];
            var range = upper.pct - lower.pct;
            var rangePct = (pct - lower.pct) / range;
            var pctUpper = 1 - rangePct;
            var pctLower = rangePct;
            var red = Math.floor(lower.color.r * pctLower + upper.color.r * pctUpper);
            var green = Math.floor(lower.color.g * pctLower + upper.color.g * pctUpper);
            if (pct > 0.5) {
                green = 0;
            }
            else
            {
                red = 0;
            }
            var color = {
                r: green,
                g: red,
                b: 0
            };
            return 'rgb(' + [color.r, color.g, color.b].join(',') + ')';
        // or output as hex if preferred
        }
    }
}  

var mouseX;
var mouseY;
var svgX;
var svgY;

function getMouseCoordinates(event)
{
    var ev = event || window.event;
    mouseX = ev.pageX;
    mouseY = ev.pageY;
}


function get_scale(value, x, y, column)
{
    if (value > 99) value = 99;
    if (value < 1) value = 0;
    
    var props = getProperties();
    
    var table = draw_gradient("vertical", defaultColumns[column]['min'], defaultColumns[column]['max'], props[column]['min'], props[column]['max'], value);

    var html = '<table style="border: 1px solid white;" cellspacing="0" cellpadding="0"><tr><td>';
    html += table;
    
    html += '</td></tr></table>';
    document.getElementById('floater').innerHTML = html;
    $('#floater').css("display",  "block");
    $('#floater').css("left",  mouseX - LOCATION.x + x + pie_rad + 5);
    $('#floater').css("top",   mouseY - LOCATION.y + y - (value * 2));
    //console.log((mouseX - LOCATION.x + x + pie_rad + 5)+ ":" + (mouseY - LOCATION.y + y - (value * 2)));
}


function get_slice_base_color(column)
{
    var properties = getProperties(); //TODO: get by column
  
    var color_func = d3.scale.category10();
    var col_array = Object.keys(properties);
  
    for (var i in col_array)
    {
        color_func(col_array[i]);
    }
  
    return d3.rgb(color_func(column));
}


function bring_circles_to_front()
{
    select_all_nodes().selectAll("circle").attr("bogus", function () {
        bring_to_front(this);
        return 0;
    });
}


function remove_metadata()
{
    delete_paths();
    columns = new Object();
    $('.metadatacheckbox').prop('checked', false);
    select_all_nodes().selectAll("text")
    .attr("display", function (d) 
    {
        if (d3.select(this).attr("num_slices") > 0)
        {
            return "none";
        }
      
        return (document.getElementById("nodetextvisiblecheckbox").checked) ? "block" : "none";
    });
  
    select_all_nodes().selectAll("text")
    .attr("y", function (d) 
    {
        return Number(d3.select(this.parentNode).select("circle").attr("cy"));
    });
}


function toggle_metadata_text_visible()
{
    var visible = document.getElementById("textvisiblecheckbox").checked;
    
    select_all_nodes().selectAll("text")
    .attr("display", function (d) 
    {
        if (d3.select(this).attr("num_slices") > 0)
        {
            return (visible) ? "block" : "none";
        }
      
        return d3.select(this).attr("display");
    });
}


function toggle_node_text_visible()
{
    var visible = document.getElementById("nodetextvisiblecheckbox").checked;
    
    select_all_nodes().selectAll("text")
    .attr("display", function (d) 
    {
        bring_to_front(this);
        if (d3.select(this).attr("num_slices") > 0)
        {
            return d3.select(this).attr("display");
        }
      
        if (metadata_mode && ! node_has_metadata(d.name))
        {
            return "none";
        }
      
        return (visible) ? "block" : "none";
    });
    select_all_nodes().selectAll("text")
    .attr("bogus", function (d) 
    {
        bring_to_front(this);
        return 0;
    });
}


function update_force()
{
    var link_dist = Number(document.getElementById('linkdisttextbox').value);
    var charge = Number(document.getElementById('chargetextbox').value);
  
    force.stop();
    force.linkDistance(link_dist);
    force.charge(charge);
    force.start();
}


function is_ratio(column)
{
    var properties = getProperties();
  
    if (properties[column]['type'].toLowerCase() == 'r')
    {
        return true;
    }
    return false;
}


function delete_paths()
{
    d3.select("#chart")
    .select("svg")
    .selectAll("g.node")
    .selectAll("text")
    .attr("bogus", function (d) {
        if (d3.select(this).attr("num_slices") > 0) d3.select(this).remove();
        return 0;
    });
            
    return d3.select("#chart")
    .select("svg")
    .selectAll("g.node")
    .selectAll("path")
    .remove();
            
}


function select_all_nodes()
{
    return d3.select("#chart")
    .select("svg")
    .selectAll("g.node");
}


function set_text_color(column)
{
    d3.select("#chart")
    .select("svg")
    .selectAll("g.node")
    .selectAll("text")
    .attr("fill",  function(d) 
    { 
        return get_text_color(column, d.name);
    });
}


function get_text_color(column, name)
{
    if (!column) return "white";
  
    var metadata = getMetadata();

    var metadata_val = metadata[column][name];
    if (metadata_val == "null") return "black";
    return "white";
}


function get_border_color(name)
{
    var metadata = getMetadata();
  
    for (var i in metadata)
    {
        if (metadata[i][name]) return "black";
        break;
    }
  
    return "#FFFFFF";
}


function resize_svg()
{
    d3.select("svg").attr("width", "100%")
    d3.selectAll("g").attr("width", "100%")
    width = getInnerWidth();
   
    force.size([width, height]);

    return;
}


function init_svg()
{
    // Find your root SVG element
    pt = document.getElementsByTagName('svg')[0].createSVGPoint();
    document.getElementsByTagName('svg')[0].addEventListener('mousemove',function(evt){
        LOCATION = cursorPoint(evt);
    // Use loc.x and loc.y here
    },false);
}

// Get point in global SVG space
function cursorPoint(evt){
    pt.x = evt.clientX;
    pt.y = evt.clientY;
    return pt.matrixTransform(document.getElementsByTagName('svg')[0].getScreenCTM().inverse());
}





$('#chart').on('mousemove', function(e) {
    svgX = $(this).position().left;
    svgY = $(this).position().top;
});
