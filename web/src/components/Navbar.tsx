import Link from "next/link";

export default function Navbar() {
    return (
        <nav>
            <Link href="#" className="logo-area">
                <i className="ph-fill ph-books logo-icon"></i>
                StudySync
            </Link>
            <div className="nav-links">
                <Link href="#features">Features</Link>
                <Link href="#downloads">Downloads</Link>
                <a href="https://github.com/salem-5/StudySync" target="_blank" rel="noopener noreferrer">GitHub</a>
            </div>
        </nav>
    );
}