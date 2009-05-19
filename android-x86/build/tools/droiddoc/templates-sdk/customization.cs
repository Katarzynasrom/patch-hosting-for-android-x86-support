<?cs # This default template file is meant to be replaced. ?>
<?cs # Use the -tempatedir arg to javadoc to set your own directory with a replacement for this file in it. ?>

<?cs 
def:custom_masthead() ?>
  <div id="header">
      <div id="headerLeft">
          <a href="<?cs var:toroot ?>index.html" tabindex="-1"><img
              src="<?cs var:toroot ?>assets/images/bg_logo.png" alt="Android Developers" /></a>
          <ul class="<?cs 
                  if:reference ?>reference<?cs
                  elif:guide ?>guide<?cs
                  elif:sdk ?>sdk<?cs
                  elif:home ?>home<?cs
                  elif:community ?>community<?cs
                  elif:publish ?>publish<?cs
                  elif:about ?>about<?cs /if ?>">
              <li id="home-link"><a href="<?cs var:toroot ?><?cs 
                  if:android.whichdoc != "online" ?>offline.html<?cs 
                  else ?>index.html<?cs /if ?>">
                  <span>Home</span></a></li>
              <li id="sdk-link"><a href="<?cs var:toroot ?>sdk/<?cs var:sdk.current ?>/index.html"><span>SDK</span></a></li>
              <li id="guide-link"><a href="<?cs var:toroot ?>guide/index.html"
                                  onClick="return loadLast('guide')"><span>Dev Guide</span></a></li>
              <li id="reference-link"><a href="<?cs var:toroot ?>reference/packages.html" 
                                  onClick="return loadLast('reference')"><span>Reference</span></a></li>
              <li><a href="http://android-developers.blogspot.com"><span>Blog</span></a></li>
              <li id="community-link"><a href="<?cs var:toroot ?>community/index.html"><span>Community</span></a></li>
          </ul>
      </div>
      <div id="headerRight">
          <div id="headerLinks">
            <!-- <img src="<?cs var:toroot ?>assets/images/icon_world.jpg" alt="" /> -->
            <span class="text">
              <!-- &nbsp;<a href="#">English</a> | -->
              <a href="http://www.android.com">Android.com</a>
            </span>
          </div><?cs 
          call:default_search_box() ?>
      </div><!-- headerRight -->
  </div><!-- header --><?cs 
/def ?><?cs # custom_masthead ?>

<?cs 
def:sdk_nav() ?>
  <div class="g-section g-tpl-240" id="body-content">
    <div class="g-unit g-first not-resizable" id="side-nav">
      <div id="devdoc-nav"><?cs 
        include:"../../../../frameworks/base/docs/html/sdk/sdk_toc.cs" ?>
      </div>
    </div> <!-- end side-nav -->
<?cs /def ?>

<?cs 
def:guide_nav() ?>
  <div class="g-section g-tpl-240" id="body-content">
    <div class="g-unit g-first side-nav-resizable" id="side-nav">
      <div id="devdoc-nav"><?cs 
        include:"../../../../frameworks/base/docs/html/guide/guide_toc.cs" ?>
      </div>
    </div> <!-- end side-nav -->
    <script>
      addLoadEvent(function() {
        scrollIntoView("devdoc-nav");
        });
    </script>
<?cs /def ?>

<?cs 
def:publish_nav() ?>
  <div class="g-section g-tpl-180" id="body-content">
    <div class="g-unit g-first" id="side-nav">
      <div id="devdoc-nav"><?cs 
        include:"../../../../frameworks/base/docs/html/publish/publish_toc.cs" ?>
      </div>
    </div> <!-- end side-nav -->
<?cs /def ?>

<?cs 
def:custom_left_nav() ?><?cs 
  if:guide ?><?cs 
    call:guide_nav() ?><?cs 
  elif:publish ?><?cs 
    call:publish_nav() ?><?cs 
  elif:sdk ?><?cs 
    call:sdk_nav() ?><?cs 
  else ?><?cs 
    call:default_left_nav() ?><?cs 
  /if ?><?cs 
/def ?>

<?cs # appears at the bottom of every page ?><?cs 
def:custom_cc_copyright() ?>
  Except as noted, this content is 
  licensed under <a href="http://creativecommons.org/licenses/by/2.5/">
  Creative Commons Attribution 2.5</a>. For details and 
  restrictions, see the <a href="<?cs var:toroot ?>license.html">Content 
  License</a>.<?cs 
/def ?>

<?cs 
def:custom_copyright() ?>
  Except as noted, this content is licensed under <a
  href="http://www.apache.org/licenses/LICENSE-2.0">Apache 2.0</a>. 
  For details and restrictions, see the <a href="<?cs var:toroot ?>license.html">
  Content License</a>.<?cs 
/def ?>

<?cs 
def:custom_footerlinks() ?>
  <p>
    <a href="http://www.android.com/terms.html">Site Terms of Service</a> -
    <a href="http://www.android.com/privacy.html">Privacy Policy</a> -
    <a href="http://www.android.com/branding.html">Brand Guidelines</a>
  </p><?cs 
/def ?>

<?cs # appears on the right side of the blue bar at the bottom off every page ?><?cs 
def:custom_buildinfo() ?>
  Android <?cs var:sdk.version ?>&nbsp;r<?cs var:sdk.rel.id ?> - <?cs var:page.now ?>
<?cs /def ?>
