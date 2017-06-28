$(document).ready(function() {
  var visualSearch = VS.init({
    container  : $('#search_box_container'),
    query      : '',
    showFacets : true,
    unquotable : ['fasta-comment', 'cluster-id', 'fasta-id', 'clique-id', 'node-count', 'has-metadata'],
    callbacks  : {
      search : function(query, searchCollection) {
        var $query = $('#search_query');
        var count  = searchCollection.size();
        $query.html('<span class="raquo">&raquo;</span> You filtered on: ' +
                    '<b>' + (query || '<i>nothing</i>') + '</b>. ' +
                    '(' + count + ' term' + (count==1 ? '' : 's') + ')');
console.log("Annoy: " + query + "\n");
console.log(searchCollection);
tmp = searchCollection.toJSON();
      },
      facetMatches : function(callback) {
        callback(['fasta-comment', 'cluster-id', 'fasta-id', 'clique-id', 'node-count', 'has-metadata']);
      },
      valueMatches : function(facet, searchTerm, callback) {
        switch (facet) {
          case 'fasta-comment':
            console.log("autocomplete_ids length:" + autocomplete_comments.length);
            if (autocomplete_comments.length < 3000) callback(autocomplete_comments);
            break;
          case 'cluster-id':
            //callback(autocomplete_ids);
            break;
          case 'fasta-id':
            //callback(autocomplete_ids);
            break;
          case 'node-count':
            //callback(['public', 'private', 'protected']);
            break;
          case 'clique-id':
            //callback(['public', 'private', 'protected']);
            break;
          case 'has-metadata':
            callback(['true']);
            break;
          default:
            break;
        }
      }
    }
  });
});
