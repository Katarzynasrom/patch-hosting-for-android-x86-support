
```
On Thu, 2009-04-30 at 10:46 +0530, Ravishankar Haranath wrote:
In the file <ANDROI_SRC>/vendor/asus/eee_701/eee_701.mk, change 'generic_with_google.mk' to 'generic.mk' in the line,
> 
> $(call inherit-product, $(SRC_TARGET_DIR)/product/generic_with_google.mk)
> 
> and try again. This is mostly because of google.maps api sources being removed from the repo and separating it as an addon stuff, in the recent changes.
> 
> 
> 
> 
> with 
> 
> On Wed, Apr 29, 2009 at 11:00 AM, Gani Bhagavathula <gani.bhagavathula@gmail.com> wrote:
> 
>         Jean:
>         
>         Thanks.  The first error during repo went away after following your
>         suggestion.  I applied the patch again after doing the repo sync.
>         
>         However, the second error during compile -  No rule to make target
>         `vendor/google/frameworks/maps/com.google.android.maps.xml' still
>         continues.  Anyone have any tips for me?
>         
>         Gani
>         
```