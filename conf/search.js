  var tmp; // fill with search params
  var autocomplete_ids      = new Array(); // fill with ids for autocomplete
  var autocomplete_comments = new Array(); // fill with comments for autocomplete
  var require_one = true;

  function requireOne()
  {
    if (! require_one)
    {
      $("#one-button").css("color", $("#one-button").css("background-color"));
      $("#one-button").css("background-color", $("#one-button").css("border-color"));
      $("#one-button").css("border-color", $("#one-button").css("color"));
  
      $("#all-button").css("color", $("#all-button").css("background-color"));
      $("#all-button").css("background-color", $("#all-button").css("border-color"));
      $("#all-button").css("border-color", $("#all-button").css("color"));
    }

    require_one = true;
  }

  function requireAll()
  {
    if (require_one)
    {
      $("#one-button").css("color", $("#one-button").css("background-color"));
      $("#one-button").css("background-color", $("#one-button").css("border-color"));
      $("#one-button").css("border-color", $("#one-button").css("color"));
  
      $("#all-button").css("color", $("#all-button").css("background-color"));
      $("#all-button").css("background-color", $("#all-button").css("border-color"));
      $("#all-button").css("border-color", $("#all-button").css("color"));
    }

    require_one = false;
  }


function filter_one()
{
  console.log(tmp);
  var map = new Object();
  for (var i in tmp)
  {
    var cat   = tmp[i].category;
    var value = tmp[i].value;
    if (map[cat] != undefined)
    {
      map[cat] += value + '|';
    }
    else
    {
      map[cat] = value + '|';
    }
  }
  for (var i in map)
  {
    if (map[i] != undefined)
    {
      map[i] = map[i].slice(0, -1);
      console.log(':' + map[i] + ':');
    }
  }


  d3.selectAll("tr.r").attr("style", "display:none");
  d3.selectAll("a")
  .attr("title", function () 
  {
    if (this.className == 'tt')
    {
      if (map['fasta-comment'] != undefined)
      {
        var re = new RegExp(map['fasta-comment'], "ig");
        if (this.title.match(re) != null)
        {
          this.parentNode.parentNode.style.display = "";
        }
      }
      if (map['fasta-id'] != undefined)
      {
          var newre = new RegExp(map['fasta-id'], "ig");
          if (this.textContent.match(newre) != null)
          {
            this.parentNode.parentNode.style.display = "";
          }
      }
      if (map['has-metadata'] != undefined)
      {
          if (hasMetadata(this.textContent).toString() == map['has-metadata'])
          {
            this.parentNode.parentNode.style.display = "";
          }
      }
    }
    return(this.title);
  });

  if (map['cluster-id'] != undefined || map['clique-id'] != undefined || map['node-count'] != undefined)
  {
    d3.selectAll("tr.r").attr("title", function () 
    {
      var textContent, ranges, text, range, low, high;

      if (map['cluster-id'] != undefined)
      {
        textContent = Number(this.childNodes[1].textContent);
        ranges = map['cluster-id'].split(/\s*[|]\s*/);
        for (var i in ranges)
        {
          text = ranges[i];
          text = text.replace("[", "");
          text = text.replace("]", "");
          range = text.split(/\s*-\s*/);
          low = Number(range[0]);
          high = Number(range[1]);
          if (textContent <= high && textContent >= low)
          {
            this.style.display = "";     
            break;
          }
        }
      }
      if (map['clique-id'] != undefined)
      {
        textContent = Number(this.childNodes[2].textContent);
        ranges = map['clique-id'].split(/\s*[|]\s*/);
        for (var i in ranges)
        {
          text = ranges[i];
          text = text.replace("[", "");
          text = text.replace("]", "");
          range = text.split(/\s*-\s*/);
          low = Number(range[0]);
          high = Number(range[1]);
          if (textContent <= high && textContent >= low)
          {
            this.style.display = "";     
            break;
          }
        }
      }

      if (map['node-count'] != undefined)
      {
        textContent = Number(this.childNodes[3].textContent);
        ranges = map['node-count'].split(/\s*[|]\s*/);
        for (var i in ranges)
        {
          text = ranges[i];
          text = text.replace("[", "");
          text = text.replace("]", "");
          range = text.split(/\s*-\s*/);
          low = Number(range[0]);
          high = Number(range[1]);
          if (textContent <= high && textContent >= low)
          {
            this.style.display = "";     
            break;
          }
        }
      }

      return this.title;
    });
  }
}


  function filter_all()
  {
        console.log(tmp);
        var map = new Object(); 
        var cluster_id_cat;
        var cluster_id_val;
        var clique_id_cat;
        var clique_id_val;
        var node_count_cat;
        var node_count_val;
        var metadata_cat;
        var metadata_val;
        for (var i in tmp)
        {
          var cat   = tmp[i].category;
          var value = tmp[i].value;
          map[cat + "-" + value] = value;
          console.log(cat + "-" + value + ": " + value);
          if (cat.search("cluster-id") >= 0) 
          {
            cluster_id_val = value;
            cluster_id_cat = cat + "-" + value;
          }
          if (cat.search("clique-id") >= 0) 
          {
            clique_id_val = value;
            clique_id_cat = cat + "-" + value;
          }
          if (cat.search("node-count") >= 0) 
          {
            node_count_val = value;
            node_count_cat = cat + "-" + value;
          }
          if (cat.search("has-metadata") >= 0) 
          {
            metadata_val = value;
            metadata_cat = cat + "-" + value;
          }
        }

        var rows = new Object();
        d3.selectAll("tr.r").attr("style", "display:none");
        d3.selectAll("a")
        .attr("title", function () 
        {
          if (this.className != "tt") return(this.title);

          var row_key = this.parentNode.parentNode.childNodes[1].textContent + ":" + this.parentNode.parentNode.childNodes[2].textContent;
          if (!rows[row_key])
          {
            rows[row_key] = new Object();

            for (var i in map)
            {
              rows[row_key][i] = false;
              rows[row_key][i] = false;
            }
          }

          for (var i in map)
          {
            if (i.search(/clique-id/)    >= 0) continue;
            if (i.search(/cluster-id/)   >= 0) continue;
            if (i.search(/node-count/)   >= 0) continue;
            if (i.search(/has-metadata/) >= 0) continue;

	    var re = new RegExp(map[i], "ig");
            var text = "";

            if (i.search("fasta-comment") >= 0) text = this.title;
            if (i.search("fasta-id")      >= 0) text = this.textContent;

	      if (text.match(re) != null)
            {
              rows[row_key][i] = true;
            }
          }

          return(this.title);
        });

        if (cluster_id_cat || clique_id_cat || node_count_cat || metadata_cat)
        {
          d3.selectAll("tr.r").attr("title", function () 
          {
            var row_key = this.childNodes[1].textContent + ":" + this.childNodes[2].textContent;
            var textContent, text, range, low, high;
            if (clique_id_cat)
            {
              textContent = Number(this.childNodes[2].textContent);
              text = clique_id_val;
              text = text.replace("[", "");
              text = text.replace("]", "");
              range = text.split(/\s*-\s*/);
              low = Number(range[0]);
              high = Number(range[1]);
              rows[row_key][clique_id_cat] = false;
              if (textContent <= high && textContent >= low)
              {
                rows[row_key][clique_id_cat] = true;
              }
            }

            if (cluster_id_cat)
            {
              textContent = Number(this.childNodes[1].textContent);
              text = cluster_id_val;
              text = text.replace("[", "");
              text = text.replace("]", "");
              range = text.split(/\s*-\s*/);
              low = Number(range[0]);
              high = Number(range[1]);
              rows[row_key][cluster_id_cat] = false;
              if (textContent <= high && textContent >= low)
              {
                rows[row_key][cluster_id_cat] = true;
              }
            }

            if (node_count_cat)
            {
              textContent = Number(this.childNodes[3].textContent);
              text = node_count_val;
              text = text.replace("[", "");
              text = text.replace("]", "");
              range = text.split(/\s*-\s*/);
              low = Number(range[0]);
              high = Number(range[1]);
              rows[row_key][node_count_cat] = false;
              if (textContent <= high && textContent >= low)
              {
                rows[row_key][node_count_cat] = true;
              }
            }

            if (metadata_cat)
            {
              rows[row_key][metadata_cat] = false;
              for (var i = 1; i < this.childNodes.length; ++i)
              {
                if (i < 5) continue;
                if (hasMetadata(this.childNodes[i].textContent).toString() == metadata_val)
                {
                  rows[row_key][metadata_cat] = true;
                }
              }
            }

            return this.title;
          });
        }

        for (var i in rows) {
          rows[i]['display'] = "";
          for (var j in map)
          {
            if (j == 'display') next;

            if (! rows[i][j])
            {
              rows[i]['display'] = "none";
              break;
            }
          }
        }


        d3.selectAll("a")
        .attr("title", function () 
        {
          if (this.className != "tt") return(this.title);

          var row_key = this.parentNode.parentNode.childNodes[1].textContent + ":" + this.parentNode.parentNode.childNodes[2].textContent;
          if (!rows[row_key]) return(this.title);

          this.parentNode.parentNode.style.display = rows[row_key]['display'];
          return(this.title);
        });


  }


function filter_tables()
{
  if (tmp != undefined)
  {
    if (tmp.length < 1)
    {
      d3.selectAll("tr.h").attr("title", function () { this.style.display = ""; return this.title;});
      d3.selectAll("tr.r").attr("title", function () { this.style.display = ""; return this.title;});
      return;
    }
    d3.selectAll("tr.h").attr("style", "display:none");

    filter_search_object();

    if (require_one)
    {
      filter_one();
    }
    else
    {
      filter_all();
    }

    //redo_headers(); // TODO: create this function
  }
  else 
  {
    // tmp == null
  }
}


function filter_search_object()
{
  for (var i in tmp)
  {
    var cat   = tmp[i].category;
    var value = tmp[i].value;

    if (cat == "cluster-id" || cat == "clique-id" || cat == "node-count")
    {
      var re = new RegExp("^\[[0-9]+-[0-9]+\]", "ig");
      if (value.match(re) == null)
      {
        var newre = new RegExp("[0-9]+-[0-9]+", "ig");
        if (value.match(newre) != null)
        {
          tmp[i].value = "[" + value + "]";
        }
        else
        {
          tmp[i].value = "[" + value + "-" + value + "]";
        }
      }
    }
  }
}


function fillAutocompleteLists()
{
  var ids      = new Object();
  var comments = new Object();

  d3.selectAll("a")
    .attr("title", function () 
    {
      if (this.className != "tt")
      {
        return this.title;
      }

      ids[this.textContent] = 1;

      var justChars = this.title.replace(/[^a-zA-Z]/g, " ");
      var splits = justChars.split(" ");
      for (var i in splits)
      {
        var word = splits[i];
        if (word.length >= 6)
        {
          comments[word] = 1;
        }
      }

      return this.title;
    });

  for (var i in comments)
  {
    autocomplete_comments.push(i);
  } 
  for (var i in ids)
  {
    autocomplete_ids.push(i);
  } 
}
