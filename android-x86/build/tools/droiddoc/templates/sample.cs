<?cs include:"doctype.cs" ?>
<?cs include:"macros.cs" ?>
<?cs set:guide="true" ?>
<html>
<?cs include:"head_tag.cs" ?>
<?cs include:"header.cs" ?>

<div class="g-unit" id="doc-content"><a name="top"></a>

<div id="jd-header" class="guide-header">

  <span class="crumb">
    <a href="<?cs var:toroot ?>guide/samples/index.html">Sample Code &gt;</a>
    
  </span>
<h1><?cs var:page.title ?></h1>
</div>

<div id="jd-content">

<p><a href="<?cs var:realFile ?>">Original <?cs var:realFile ?></a></p>

<!-- begin file contents -->
<pre class="Code prettyprint"><?cs var:fileContents ?></pre>
<!-- end file contents -->

<?cs include:"footer.cs" ?>
</div><!-- end jd-content -->
</div> <!-- end doc-content -->

<?cs include:"trailer.cs" ?>

</body>
</html>
