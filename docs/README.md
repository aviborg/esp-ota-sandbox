# ESP8266 OTA Sandbox - Documentation

This folder contains the GitHub Pages documentation for the ESP8266 OTA Sandbox project.

## GitHub Pages Setup

This documentation is hosted on GitHub Pages and is configured to serve from the `/docs` folder on the main branch.

### Files

- `index.html` - Main documentation page with comprehensive project information
- `style.css` - Modern, responsive stylesheet for the documentation

### Viewing the Documentation

The documentation is available at: https://aviborg.github.io/esp-ota-sandbox/

### Local Development

To preview the documentation locally:

```bash
cd docs
python3 -m http.server 8080
```

Then open http://localhost:8080 in your browser.

### Updating the Documentation

To update the documentation:

1. Edit `index.html` or `style.css`
2. Test locally using the commands above
3. Commit and push your changes
4. GitHub Pages will automatically update within a few minutes

## Features

The documentation includes:

- ✅ Modern, responsive design
- ✅ Clear navigation with anchor links
- ✅ Comprehensive feature descriptions
- ✅ Step-by-step quick start guide
- ✅ Detailed "How It Works" section
- ✅ CI/CD workflow documentation
- ✅ Troubleshooting guide
- ✅ Code examples with syntax highlighting
- ✅ Mobile-friendly layout
- ✅ Print-friendly styles

## Customization

To customize the appearance:

- Colors and theme: Edit CSS variables in `style.css` (`:root` section)
- Content: Update sections in `index.html`
- Layout: Modify CSS grid and flexbox settings in `style.css`
