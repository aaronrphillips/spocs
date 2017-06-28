var head         = document.getElementsByTagName("head")[0];
var curr_idx     = 0;


function script_load()
{
  if (curr_idx == scripts.length)
  {
    console.log("scripts finished");
    body_load();
    return;
  }
  var tag     = document.createElement('script');
  tag.src     = scripts[curr_idx]; ++curr_idx;
  tag.type    = "text/javascript";
  tag.charset = "utf-8";
  tag.onload = script_load;
//console.log(tag);
  head.appendChild(tag);
}


function css_load()
{
  if (curr_idx == stylesheets.length)
  {
    console.log("stylesheets finished");
    curr_idx = 0;
    script_load();
    return;
  }
  var tag    = document.createElement('link');
  tag.href   = stylesheets[curr_idx]; ++curr_idx;
  tag.media  = "screen";
  tag.rel    = "stylesheet";
  tag.type   = "text/css";
  tag.onload = css_load;
//console.log(tag)
  head.appendChild(tag)
}


//  NOTE: Load CSS files first in case JavaScript overrides the CSS
css_load();
