// Shared theme logic for all feature pages
const themes = {
    purple: {
        "--primary": "#6c35a8", "--primary-dark": "#4e1f80", "--primary-light": "#9b6fd4",
        "--accent": "#c084fc", "--bg": "#faf7ff", "--bg-card": "#ffffff", "--bg-section": "#f3eaff",
        "--text-head": "#1a0a2e", "--text-body": "#3d2b5a", "--text-muted": "#8a6fb0",
        "--border": "rgba(108,53,168,0.15)", "--shadow": "0 8px 32px rgba(108,53,168,0.12)",
        "--shadow-hover": "0 16px 48px rgba(108,53,168,0.22)"
    },
    saffron: {
        "--primary": "#ff7a00", "--primary-dark": "#cc5f00", "--primary-light": "#ffb266",
        "--accent": "#ff9933", "--bg": "#fff9f2", "--bg-card": "#ffffff", "--bg-section": "#fff2e6",
        "--text-head": "#2b1300", "--text-body": "#5a3a1a", "--text-muted": "#a67c52",
        "--border": "rgba(255,122,0,0.18)", "--shadow": "0 8px 32px rgba(255,122,0,0.15)",
        "--shadow-hover": "0 16px 48px rgba(255,122,0,0.28)"
    },
    neon: {
        "--primary": "#ff2e88", "--primary-dark": "#c6005c", "--primary-light": "#ff7bb8",
        "--accent": "#00e5ff", "--bg": "#0f0820", "--bg-card": "#17102f", "--bg-section": "#120a26",
        "--text-head": "#ffffff", "--text-body": "#d6d2ff", "--text-muted": "#8f88c7",
        "--border": "rgba(255,255,255,0.08)", "--shadow": "0 8px 32px rgba(255,46,136,0.25)",
        "--shadow-hover": "0 16px 48px rgba(0,229,255,0.35)"
    }
};

function applyTheme(name) {
    const t = themes[name];
    if (!t) return;
    Object.keys(t).forEach(k => document.documentElement.style.setProperty(k, t[k]));
    document.body.classList.remove('theme-purple','theme-saffron','theme-neon');
    document.body.classList.add('theme-'+name);
}

document.addEventListener('DOMContentLoaded', () => {
    applyTheme(localStorage.getItem('theme') || 'saffron');
});
