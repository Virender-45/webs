/* ================================================================
   PORTFOLIO — script.js
   ================================================================ */

/* ----------------------------------------------------------------
   1. HAMBURGER MENU — removed (nav hidden on mobile/tablet)
   ---------------------------------------------------------------- */


/* ----------------------------------------------------------------
   2. CONTACT FORM VALIDATION
   ---------------------------------------------------------------- */
document.getElementById("contactForm").addEventListener("submit", async function (e) {
    e.preventDefault();

    const name = document.getElementById("name").value.trim();
    const email = document.getElementById("email").value.trim();
    const subject = document.getElementById("subject").value.trim();
    const message = document.getElementById("message").value.trim();
    const msg = document.getElementById("formMsg");

    if (name.length < 3) {
        showFormMsg(msg, "⚠ Name must be at least 3 characters.", "error");
        return;
    }

    const btn = this.querySelector("button[type='submit']");
    btn.innerHTML = 'Sending...';
    btn.disabled = true;

    try {
        const res = await fetch("/contact", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({ name, email, subject, message })
        });

        const data = await res.json();

        if (res.ok) {
            showFormMsg(msg, "✅ Message saved!", "success");
            this.reset();
        } else {
            showFormMsg(msg, data.error || "Something went wrong", "error");
        }

    } catch (err) {
        showFormMsg(msg, "Server error", "error");
    }

    btn.innerHTML = 'Send Message';
    btn.disabled = false;
});

function showFormMsg(el, text, type) {
    el.textContent = text;
    el.className = 'form-msg ' + type;
}

function shake(el) {
    el.classList.add('shake');
    el.addEventListener('animationend', () => el.classList.remove('shake'), { once: true });
}


/* ----------------------------------------------------------------
   3. TYPING EFFECT — hero role
   ---------------------------------------------------------------- */
const roles = ["Software Developer", "Web Developer", "MCA Student", "Problem Solver"];
let roleIndex = 0;
let charIndex = 0;
let isDeleting = false;
const roleEl = document.querySelector('.hero-role');

function typeRole() {
    if (!roleEl) return;
    const current = roles[roleIndex];

    if (isDeleting) {
        roleEl.textContent = current.substring(0, charIndex--);
    } else {
        roleEl.textContent = current.substring(0, charIndex++);
    }

    let delay = isDeleting ? 60 : 110;

    if (!isDeleting && charIndex === current.length + 1) {
        delay = 1800;
        isDeleting = true;
    } else if (isDeleting && charIndex === 0) {
        isDeleting = false;
        roleIndex = (roleIndex + 1) % roles.length;
        delay = 400;
    }

    setTimeout(typeRole, delay);
}

if (roleEl) {
    roleEl.classList.add('typing-cursor');
    setTimeout(typeRole, 800);
}


/* ----------------------------------------------------------------
   4. SCROLL-TRIGGERED SECTION ANIMATIONS
   ---------------------------------------------------------------- */
const animatedEls = document.querySelectorAll(
    '.feature-card, .edu-inner-box, .ex-info, .det, .skill-item, .contact-intro, .contact-form-wrapper'
);

animatedEls.forEach(el => el.classList.add('scroll-hidden'));

const scrollObserver = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.classList.add('scroll-visible');
            entry.target.classList.remove('scroll-hidden');
            scrollObserver.unobserve(entry.target);
        }
    });
}, { threshold: 0.12 });

animatedEls.forEach(el => scrollObserver.observe(el));


/* ----------------------------------------------------------------
   5. ANIMATED SKILL BARS
   ---------------------------------------------------------------- */
const skillFills = document.querySelectorAll('.skill-fill');
const skillObserver = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            const fill = entry.target;
            fill.style.width = '0%';
            requestAnimationFrame(() => {
                requestAnimationFrame(() => {
                    fill.style.width = fill.dataset.width;
                });
            });
            skillObserver.unobserve(fill);
        }
    });
}, { threshold: 0.2 });

skillFills.forEach(fill => {
    const inlineWidth = fill.getAttribute('style').match(/width\s*:\s*([^;]+)/);
    fill.dataset.width = inlineWidth ? inlineWidth[1].trim() : '0%';
    fill.style.width = '0%';
    skillObserver.observe(fill);
});


/* ----------------------------------------------------------------
   6. ACTIVE NAV LINK ON SCROLL
   ---------------------------------------------------------------- */
const sections = document.querySelectorAll('section[id]');
const navLinks = document.querySelectorAll('.nav a, .footer-nav a');

const navObserver = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            navLinks.forEach(link => {
                link.classList.remove('nav-active');
                if (link.getAttribute('href') === '#' + entry.target.id) {
                    link.classList.add('nav-active');
                }
            });
        }
    });
}, { threshold: 0.35 });

sections.forEach(sec => navObserver.observe(sec));


/* ----------------------------------------------------------------
   7. SCROLL-TO-TOP BUTTON  (inline in HTML via script.js)
   ---------------------------------------------------------------- */
// (button added via HTML if present — logic handled by scroll event)
window.addEventListener('scroll', () => {
    const btn = document.getElementById('scrollTopBtn');
    if (btn) {
        btn.classList.toggle('visible', window.scrollY > 400);
    }
});

const scrollTopBtn = document.getElementById('scrollTopBtn');
if (scrollTopBtn) {
    scrollTopBtn.addEventListener('click', () => window.scrollTo({ top: 0, behavior: 'smooth' }));
}


/* ----------------------------------------------------------------
   8. LIVE CHARACTER COUNTER (message textarea)
   ---------------------------------------------------------------- */
const textarea = document.getElementById('message');
const maxChars = 500;

if (textarea) {
    const counter = document.createElement('div');
    counter.id = 'charCounter';
    counter.textContent = `0 / ${maxChars}`;
    textarea.parentNode.appendChild(counter);

    textarea.addEventListener('input', () => {
        const len = textarea.value.length;
        if (len > maxChars) textarea.value = textarea.value.substring(0, maxChars);
        updateCharCount(Math.min(len, maxChars));
    });
}

function updateCharCount(len) {
    const counter = document.getElementById('charCounter');
    if (!counter) return;
    counter.textContent = `${len} / ${maxChars}`;
    counter.style.color = len >= maxChars ? 'var(--primary)' : 'var(--text-muted)';
}


/* ----------------------------------------------------------------
   9. THEME SWITCHER  ← UPDATED: adds body class for neon fix
   ---------------------------------------------------------------- */
const themes = {

    purple: {
        "--primary": "#6c35a8",
        "--primary-dark": "#4e1f80",
        "--primary-light": "#9b6fd4",
        "--accent": "#c084fc",
        "--accent-soft": "#f3e8ff",
        "--bg": "#faf7ff",
        "--bg-card": "#ffffff",
        "--bg-section": "#f3eaff",
        "--text-head": "#1a0a2e",
        "--text-body": "#3d2b5a",
        "--text-muted": "#8a6fb0",
        "--border": "rgba(108, 53, 168, 0.15)",
        "--shadow": "0 8px 32px rgba(108, 53, 168, 0.12)",
        "--shadow-hover": "0 16px 48px rgba(108, 53, 168, 0.22)",
        "--hero-img-after": "linear-gradient(to top, rgba(108,53,168,0.55), transparent)"
    },

    saffron: {
        "--primary": "#ff7a00",
        "--primary-dark": "#cc5f00",
        "--primary-light": "#ffb266",
        "--accent": "#ff9933",
        "--accent-soft": "#fff2e6",
        "--bg": "#fff9f2",
        "--bg-card": "#ffffff",
        "--bg-section": "#fff2e6",
        "--text-head": "#2b1300",
        "--text-body": "#5a3a1a",
        "--text-muted": "#a67c52",
        "--border": "rgba(255,122,0,0.18)",
        "--shadow": "0 8px 32px rgba(255,122,0,0.15)",
        "--shadow-hover": "0 16px 48px rgba(255,122,0,0.28)",
        "--hero-img-after": "linear-gradient(to top, rgba(255,122,0,0.55), transparent)"
    },

    neon: {
        "--primary": "#ff2e88",
        "--primary-dark": "#c6005c",
        "--primary-light": "#ff7bb8",
        "--accent": "#00e5ff",
        "--bg": "#0f0820",
        "--bg-card": "#17102f",
        "--bg-section": "#120a26",
        "--text-head": "#ffffff",
        "--text-body": "#d6d2ff",
        "--text-muted": "#8f88c7",
        "--border": "rgba(255,255,255,0.08)",
        "--shadow": "0 8px 32px rgba(255,46,136,0.25)",
        "--shadow-hover": "0 16px 48px rgba(0,229,255,0.35)",
        "--hero-img-after": "linear-gradient(to top, rgba(255,46,136,0.55), rgba(106,92,255,0.35), transparent)"
    }

};

function setTheme(themeName) {
    const theme = themes[themeName];
    const root = document.documentElement;

    Object.keys(theme).forEach(key => {
        root.style.setProperty(key, theme[key]);
    });

    // Add body class so neon-specific CSS overrides fire (header/footer bg fix)
    document.body.classList.remove('theme-purple', 'theme-saffron', 'theme-neon');
    document.body.classList.add('theme-' + themeName);

    localStorage.setItem("theme", themeName);
}

window.onload = () => {
    const savedTheme = localStorage.getItem("theme") || "saffron";
    setTheme(savedTheme);
};

const themeOrder = ["purple", "saffron", "neon"];

function toggleTheme() {
    let current = localStorage.getItem("theme") || "saffron";
    let nextIndex = (themeOrder.indexOf(current) + 1) % themeOrder.length;
    setTheme(themeOrder[nextIndex]);
}