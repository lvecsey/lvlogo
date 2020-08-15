Take a logo such as an image of initials,
and place it on a 2D particle wave, then render viewing basically top down.

Requires Image Magick utilities.

First, generate the flat logo.

```console
./gen_flatlogo.sh LV
```

Replacing the LV with your own initials.

# Compiling

```console
make lvlogo
```

# Rendering

```console
make /tmp/output_lvlogo.png
```

[Sample image](https://phrasep.com/~lvecsey/software/lvlogo/output_lvlogo.png)

![Image of output](https://phrasep.com/~lvecsey/software/lvlogo/output_lvlogo.png)


